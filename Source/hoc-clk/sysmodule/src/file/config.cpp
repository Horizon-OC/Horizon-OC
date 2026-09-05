/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <i2c.h>
#include <map>
#include <max17050.h>
#include <minIni.h>
#include <sstream>
#include <string>
#include <tmp451.h>
#include <unistd.h>

#include "../board/board.hpp"
#include "../hos/apm_ext.h"
#include "config.hpp"
#include "errors.hpp"
#include "file_utils.hpp"
#include <initializer_list>
#include <ipc_server.h>
#include <lockable_mutex.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace config {

    uint64_t configValues[HocClkConfigValue_EnumMax];

    namespace {

        bool gLoaded = false;
        std::string gSettingsPath;
        std::string gKipPath;
        std::string gProfilesDir;
        std::string gLegacyPath;
        time_t gSettingsMtime = 0;
        time_t gKipMtime = 0;
        std::atomic_bool gEnabled{ false };
        std::uint32_t gOverrideFreqs[HocClkModule_EnumMax];
        std::map<std::tuple<std::uint64_t, HocClkProfile, HocClkModule>, std::uint32_t> gProfileMHzMap;
        std::map<std::uint64_t, std::uint8_t> gProfileCountMap;
        std::set<std::uint64_t> gLoadedTids;
        std::map<std::uint64_t, time_t> gProfileMtimes;
        std::uint64_t gCurrentGameTid = 0;
        bool gProfilesDirty = false;
        bool gMigrationHappened = false;
        LockableMutex gConfigMutex;
        LockableMutex gOverrideMutex;

        time_t CheckFileMtime(const std::string &path) {
            time_t mtime = 0;
            struct stat st;
            if (stat(path.c_str(), &st) == 0) {
                mtime = st.st_mtime;
            }
            return mtime;
        }

        std::uint32_t FindClockMHz(std::uint64_t tid, HocClkModule module, HocClkProfile profile) {
            auto it = gProfileMHzMap.find(std::make_tuple(tid, profile, module));
            if (it != gProfileMHzMap.end()) {
                return it->second;
            }
            return 0;
        }

        std::uint32_t FindClockHzFromProfiles(std::uint64_t tid, HocClkModule module, std::initializer_list<HocClkProfile> profiles,
                                              u32 mhzMultiplier = 1000000) {
            std::uint32_t mhz = 0;

            if (gLoaded) {
                for (auto profile : profiles) {
                    mhz = FindClockMHz(tid, module, profile);
                    if (mhz) {
                        break;
                    }
                }
            }

            return std::max((std::uint32_t)0, mhz * mhzMultiplier);
        }

        int BrowseSettingsIni(const char *section, const char *key, const char *value, void *userdata) {
            (void)userdata;
            if (strcmp(section, CONFIG_VAL_SECTION) != 0) {
                return 1;
            }

            for (unsigned int kval = 0; kval < HocClkConfigValue_EnumMax; kval++) {
                if (hocclkIsKipConfigValue((HocClkConfigValue)kval)) {
                    continue;
                }
                if (!strcmp(key, hocclkFormatConfigValue((HocClkConfigValue)kval, false))) {
                    std::uint64_t input = strtoul(value, NULL, 0);
                    if (!hocclkValidConfigValue((HocClkConfigValue)kval, input)) {
                        input = hocclkDefaultConfigValue((HocClkConfigValue)kval);
                        fileUtils::LogLine("[cfg] Invalid value for key '%s': using default %llu", key, input);
                    }
                    configValues[kval] = input;
                    return 1;
                }
            }

            fileUtils::LogLine("[cfg] Skipping key '%s' in settings: Unrecognized config value", key);
            return 1;
        }

        int BrowseKipIni(const char *section, const char *key, const char *value, void *userdata) {
            (void)userdata;
            if (strcmp(section, CONFIG_VAL_SECTION) != 0) {
                return 1;
            }

            for (unsigned int kval = 0; kval < HocClkConfigValue_EnumMax; kval++) {
                if (!hocclkIsKipConfigValue((HocClkConfigValue)kval)) {
                    continue;
                }
                if (!strcmp(key, hocclkFormatConfigValue((HocClkConfigValue)kval, false))) {
                    std::uint64_t input = strtoul(value, NULL, 0);
                    if (!hocclkValidConfigValue((HocClkConfigValue)kval, input)) {
                        input = hocclkDefaultConfigValue((HocClkConfigValue)kval);
                        fileUtils::LogLine("[cfg] Invalid value for key '%s': using default %llu", key, input);
                    }
                    configValues[kval] = input;
                    return 1;
                }
            }

            fileUtils::LogLine("[cfg] Skipping key '%s' in kip: Unrecognized config value", key);
            return 1;
        }

        int BrowseProfileIni(const char *section, const char *key, const char *value, void *userdata) {
            std::uint64_t tid = (std::uint64_t)(uintptr_t)userdata;

            HocClkProfile parsedProfile = HocClkProfile_EnumMax;
            HocClkModule parsedModule = HocClkModule_EnumMax;

            for (unsigned int profile = 0; profile < HocClkProfile_EnumMax; profile++) {
                const char *profileCode = board::GetProfileName((HocClkProfile)profile, false);
                size_t profileCodeLen = strlen(profileCode);

                if (!strncmp(key, profileCode, profileCodeLen) && key[profileCodeLen] == '_') {
                    const char *subkey = key + profileCodeLen + 1;

                    for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                        const char *moduleCode = board::GetModuleName((HocClkModule)module, false);
                        size_t moduleCodeLen = strlen(moduleCode);
                        if (!strncmp(subkey, moduleCode, moduleCodeLen) && subkey[moduleCodeLen] == '\0') {
                            parsedProfile = (HocClkProfile)profile;
                            parsedModule = (HocClkModule)module;
                        }
                    }
                }
            }

            if (parsedModule == HocClkModule_EnumMax || parsedProfile == HocClkProfile_EnumMax) {
                return 1;
            }

            std::uint32_t mhz = strtoul(value, NULL, 10);
            if (!mhz) {
                return 1;
            }

            gProfileMHzMap[std::make_tuple(tid, parsedProfile, parsedModule)] = mhz;
            auto it = gProfileCountMap.find(tid);
            if (it == gProfileCountMap.end()) {
                gProfileCountMap[tid] = 1;
            } else {
                it->second++;
            }

            return 1;
        }

        void Close() {
            gLoaded = false;
            gProfileMHzMap.clear();
            gProfileCountMap.clear();
            gLoadedTids.clear();
            gProfileMtimes.clear();
            gCurrentGameTid = 0;
            gProfilesDirty = false;
            for (unsigned int i = 0; i < HocClkConfigValue_EnumMax; i++) {
                configValues[i] = hocclkDefaultConfigValue((HocClkConfigValue)i);
            }
        }

        void LoadSettings() {
            fileUtils::LogLine("[cfg] Reading %s", gSettingsPath.c_str());
            gSettingsMtime = CheckFileMtime(gSettingsPath);
            if (!gSettingsMtime) {
                fileUtils::LogLine("[cfg] Settings file not found, using defaults");
                return;
            }
            if (!ini_browse(&BrowseSettingsIni, nullptr, gSettingsPath.c_str())) {
                fileUtils::LogLine("[cfg] Error loading settings file");
            }
        }

        void LoadKip() {
            fileUtils::LogLine("[cfg] Reading %s", gKipPath.c_str());
            gKipMtime = CheckFileMtime(gKipPath);
            if (!gKipMtime) {
                fileUtils::LogLine("[cfg] KIP config file not found, using defaults");
                return;
            }
            if (!ini_browse(&BrowseKipIni, nullptr, gKipPath.c_str())) {
                fileUtils::LogLine("[cfg] Error loading KIP config file");
            }
        }

        std::string ProfilePathForTid(std::uint64_t tid) {
            char filename[32];
            snprintf(filename, sizeof(filename), "%016lX.ini", tid);
            return gProfilesDir + "/" + filename;
        }

        void EvictProfile(std::uint64_t tid) {
            if (tid == HOCCLK_GLOBAL_PROFILE_TID) {
                return;
            }

            for (unsigned int profile = 0; profile < HocClkProfile_EnumMax; profile++) {
                for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                    gProfileMHzMap.erase(std::make_tuple(tid, (HocClkProfile)profile, (HocClkModule)module));
                }
            }
            gProfileCountMap.erase(tid);
            gProfileMtimes.erase(tid);
            gLoadedTids.erase(tid);
        }

        void LoadProfileTid(std::uint64_t tid) {
            if (tid == 0) {
                return;
            }

            if (gLoadedTids.count(tid)) {
                return;
            }

            if (tid != HOCCLK_GLOBAL_PROFILE_TID && gCurrentGameTid != 0) {
                EvictProfile(gCurrentGameTid);
            }

            std::string path = ProfilePathForTid(tid);

            struct stat st;
            if (stat(path.c_str(), &st) == 0) {
                fileUtils::LogLine("[cfg] Loading profile %s", path.c_str());
                ini_browse(&BrowseProfileIni, (void *)(uintptr_t)tid, path.c_str());
                gProfileMtimes[tid] = st.st_mtime;
            } else {
                gProfileMtimes.erase(tid);
            }
            gLoadedTids.insert(tid);
            if (tid != HOCCLK_GLOBAL_PROFILE_TID) {
                gCurrentGameTid = tid;
            }
        }

        void Load() {
            gLoaded = false;
            for (unsigned int i = 0; i < HocClkConfigValue_EnumMax; i++) {
                configValues[i] = hocclkDefaultConfigValue((HocClkConfigValue)i);
            }
            LoadSettings();
            LoadKip();
            gLoaded = true;
        }

        struct MigrationData {
            std::vector<std::pair<std::string, std::string>> settingsKeys;
            std::vector<std::pair<std::string, std::string>> kipKeys;
            std::vector<std::pair<std::uint64_t, std::pair<std::string, std::string>>> profileEntries;
        };

        int MigrationBrowseFunc(const char *section, const char *key, const char *value, void *userdata) {
            MigrationData *data = static_cast<MigrationData *>(userdata);

            if (!strcmp(section, CONFIG_VAL_SECTION)) {
                for (unsigned int kval = 0; kval < HocClkConfigValue_EnumMax; kval++) {
                    if (!strcmp(key, hocclkFormatConfigValue((HocClkConfigValue)kval, false))) {
                        if (hocclkIsKipConfigValue((HocClkConfigValue)kval)) {
                            data->kipKeys.push_back({key, value});
                            return 1;
                        }
                        data->settingsKeys.push_back({key, value});
                        return 1;
                    }
                }
                fileUtils::LogLine("[cfg]   Skipping unrecognized key '%s'", key);
            } else {
                std::uint64_t tid = strtoul(section, NULL, 16);
                if (tid && strlen(section) == 16) {
                    data->profileEntries.push_back({tid, {key, value}});
                }
            }
            return 1;
        }

        void MigrateLegacyConfig() {
            struct stat st;
            if (stat(gLegacyPath.c_str(), &st) != 0) {
                return;
            }

            fileUtils::LogLine("[cfg] Migrating legacy config.ini");
            fileUtils::LogLine("[cfg] Legacy path: %s", gLegacyPath.c_str());
            fileUtils::LogLine("[cfg] Settings path: %s", gSettingsPath.c_str());
            fileUtils::LogLine("[cfg] Profiles dir: %s", gProfilesDir.c_str());

            MigrationData data;
            fileUtils::LogLine("[cfg] Parsing legacy config.ini...");
            ini_browse(&MigrationBrowseFunc, &data, gLegacyPath.c_str());
            fileUtils::LogLine("[cfg] Found %zu settings keys, %zu KIP keys, %zu profile entries",
                               data.settingsKeys.size(), data.kipKeys.size(), data.profileEntries.size());

            if (!data.settingsKeys.empty()) {
                fileUtils::LogLine("[cfg] Creating kip directory: %s", FILE_KIP_DIR);
                mkdir(FILE_KIP_DIR, 0777);
                fileUtils::LogLine("[cfg] Writing %zu settings to %s", data.settingsKeys.size(), gSettingsPath.c_str());

                std::vector<std::string> sKeys;
                std::vector<std::string> sVals;
                sKeys.reserve(data.settingsKeys.size());
                sVals.reserve(data.settingsKeys.size());
                for (const auto &entry : data.settingsKeys) {
                    fileUtils::LogLine("[cfg]   %s = %s", entry.first.c_str(), entry.second.c_str());
                    sKeys.push_back(entry.first);
                    sVals.push_back(entry.second);
                }

                std::vector<const char *> keyPtrs;
                std::vector<const char *> valPtrs;
                keyPtrs.reserve(sKeys.size() + 1);
                valPtrs.reserve(sVals.size() + 1);
                for (size_t i = 0; i < sKeys.size(); i++) {
                    keyPtrs.push_back(sKeys[i].c_str());
                    valPtrs.push_back(sVals[i].c_str());
                }
                keyPtrs.push_back(NULL);
                valPtrs.push_back(NULL);

                if (!ini_putsection(CONFIG_VAL_SECTION, keyPtrs.data(), valPtrs.data(), gSettingsPath.c_str())) {
                    fileUtils::LogLine("[cfg] FAILED to write settings section");
                } else {
                    fileUtils::LogLine("[cfg] Settings migration done");
                }
            } else {
                fileUtils::LogLine("[cfg] No settings keys to migrate");
            }

            if (!data.kipKeys.empty()) {
                fileUtils::LogLine("[cfg] Writing %zu KIP keys to %s", data.kipKeys.size(), gKipPath.c_str());

                std::vector<std::string> kKeys;
                std::vector<std::string> kVals;
                kKeys.reserve(data.kipKeys.size());
                kVals.reserve(data.kipKeys.size());
                for (const auto &entry : data.kipKeys) {
                    fileUtils::LogLine("[cfg]   %s = %s", entry.first.c_str(), entry.second.c_str());
                    kKeys.push_back(entry.first);
                    kVals.push_back(entry.second);
                }

                std::vector<const char *> kKeyPtrs;
                std::vector<const char *> kValPtrs;
                kKeyPtrs.reserve(kKeys.size() + 1);
                kValPtrs.reserve(kVals.size() + 1);
                for (size_t i = 0; i < kKeys.size(); i++) {
                    kKeyPtrs.push_back(kKeys[i].c_str());
                    kValPtrs.push_back(kVals[i].c_str());
                }
                kKeyPtrs.push_back(NULL);
                kValPtrs.push_back(NULL);

                if (!ini_putsection(CONFIG_VAL_SECTION, kKeyPtrs.data(), kValPtrs.data(), gKipPath.c_str())) {
                    fileUtils::LogLine("[cfg] FAILED to write KIP section");
                } else {
                    fileUtils::LogLine("[cfg] KIP migration done");
                }

                for (const auto &entry : data.kipKeys) {
                    for (unsigned int kval = 0; kval < HocClkConfigValue_EnumMax; kval++) {
                        if (!hocclkIsKipConfigValue((HocClkConfigValue)kval)) {
                            continue;
                        }
                        if (!strcmp(entry.first.c_str(), hocclkFormatConfigValue((HocClkConfigValue)kval, false))) {
                            std::uint64_t input = strtoul(entry.second.c_str(), NULL, 0);
                            if (hocclkValidConfigValue((HocClkConfigValue)kval, input)) {
                                configValues[kval] = input;
                            }
                            break;
                        }
                    }
                }
            } else {
                fileUtils::LogLine("[cfg] No KIP keys to migrate");
            }

            fileUtils::LogLine("[cfg] Creating profiles directory: %s", FILE_PROFILES_DIR);
            mkdir(FILE_PROFILES_DIR, 0777);

            std::map<std::uint64_t, std::vector<std::pair<std::string, std::string>>> profileBuckets;
            for (const auto &entry : data.profileEntries) {
                profileBuckets[entry.first].push_back(entry.second);
            }

            fileUtils::LogLine("[cfg] Writing %zu profiles (%zu TIDs)", data.profileEntries.size(), profileBuckets.size());
            for (const auto &bucket : profileBuckets) {
                char filename[32];
                snprintf(filename, sizeof(filename), "%016lX.ini", bucket.first);
                std::string profilePath = gProfilesDir + "/" + filename;

                fileUtils::LogLine("[cfg]   TID=%016lX %zu keys -> %s", bucket.first, bucket.second.size(), profilePath.c_str());

                std::vector<std::string> pKeys;
                std::vector<std::string> pVals;
                pKeys.reserve(bucket.second.size());
                pVals.reserve(bucket.second.size());
                for (const auto &kv : bucket.second) {
                    fileUtils::LogLine("[cfg]     %s = %s", kv.first.c_str(), kv.second.c_str());
                    pKeys.push_back(kv.first);
                    pVals.push_back(kv.second);
                }

                std::vector<const char *> keyPtrs;
                std::vector<const char *> valPtrs;
                keyPtrs.reserve(pKeys.size() + 1);
                valPtrs.reserve(pVals.size() + 1);
                for (size_t i = 0; i < pKeys.size(); i++) {
                    keyPtrs.push_back(pKeys[i].c_str());
                    valPtrs.push_back(pVals[i].c_str());
                }
                keyPtrs.push_back(NULL);
                valPtrs.push_back(NULL);

                if (!ini_putsection(CONFIG_VAL_SECTION, keyPtrs.data(), valPtrs.data(), profilePath.c_str())) {
                    fileUtils::LogLine("[cfg]   FAILED to write profile for TID %016lX", bucket.first);
                }
            }
            fileUtils::LogLine("[cfg] Profiles migration done");

            fileUtils::LogLine("[cfg] Renaming legacy config to config.ini.migrated");
            if (rename(gLegacyPath.c_str(), (gLegacyPath + ".migrated").c_str()) != 0) {
                fileUtils::LogLine("[cfg] WARNING: Failed to rename legacy config file");
            }
            gMigrationHappened = true;
            fileUtils::LogLine("[cfg] Migration complete");
        }

    }  // namespace

    bool IsProfileDirty() {
        return gProfilesDirty;
    }

    void SetProfileDirty(bool dirty) {
        gProfilesDirty = dirty;
    }

    void Initialize() {
        gSettingsPath = FILE_SETTINGS_PATH;
        gKipPath = FILE_KIP_CONFIG_PATH;
        gProfilesDir = FILE_PROFILES_DIR;
        gLegacyPath = FILE_LEGACY_CONFIG_PATH;
        gLoaded = false;
        gMigrationHappened = false;
        gProfileMHzMap.clear();
        gProfileCountMap.clear();
        gLoadedTids.clear();
        gProfileMtimes.clear();
        gCurrentGameTid = 0;
        gProfilesDirty = false;
        gSettingsMtime = 0;
        gKipMtime = 0;
        gEnabled = false;
        for (unsigned int i = 0; i < HocClkModule_EnumMax; i++) {
            gOverrideFreqs[i] = 0;
        }
        for (unsigned int i = 0; i < HocClkConfigValue_EnumMax; i++) {
            configValues[i] = hocclkDefaultConfigValue((HocClkConfigValue)i);
        }

        mkdir(FILE_CONFIG_DIR, 0777);
        mkdir(FILE_KIP_DIR, 0777);
        mkdir(FILE_PROFILES_DIR, 0777);

        MigrateLegacyConfig();

        gLoaded = true;
        LoadProfileTid(HOCCLK_GLOBAL_PROFILE_TID);
    }

    void Exit() {
        std::scoped_lock lock{ gConfigMutex };
        Close();
    }

    bool Refresh() {
        std::scoped_lock lock{ gConfigMutex };
        time_t settingsMtime = CheckFileMtime(gSettingsPath);
        time_t kipMtime = CheckFileMtime(gKipPath);
        if (!gLoaded || gSettingsMtime != settingsMtime || gKipMtime != kipMtime) {
            Load();
            return true;
        }
        if (IsProfileDirty()) {
            SetProfileDirty(false);
            return true;
        }

        std::vector<std::uint64_t> staleTids;
        for (std::uint64_t tid : gLoadedTids) {
            time_t mtime = CheckFileMtime(ProfilePathForTid(tid));
            auto it = gProfileMtimes.find(tid);
            time_t known = (it != gProfileMtimes.end()) ? it->second : 0;
            if (mtime != known) {
                staleTids.push_back(tid);
            }
        }
        for (std::uint64_t tid : staleTids) {
            fileUtils::LogLine("[cfg] Profile %016lX changed on disk, dropping cache", tid);
            for (unsigned int profile = 0; profile < HocClkProfile_EnumMax; profile++) {
                for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                    gProfileMHzMap.erase(std::make_tuple(tid, (HocClkProfile)profile, (HocClkModule)module));
                }
            }
            gProfileCountMap.erase(tid);
            gProfileMtimes.erase(tid);
            gLoadedTids.erase(tid);
        }
        return !staleTids.empty();
    }

    bool HasProfilesLoaded() {
        std::scoped_lock lock{ gConfigMutex };
        return gLoaded;
    }

    std::uint32_t GetAutoClockHz(std::uint64_t tid, HocClkModule module, HocClkProfile profile, bool returnRaw) {
        std::scoped_lock lock{ gConfigMutex };
        if (gLoaded && !gLoadedTids.count(tid)) {
            LoadProfileTid(tid);
        }
        switch (profile) {
            case HocClkProfile_Handheld:
                return FindClockHzFromProfiles(tid, module, { HocClkProfile_Handheld }, returnRaw ? 1 : 1000000);
            case HocClkProfile_HandheldCharging:
            case HocClkProfile_HandheldChargingUSB:
                return FindClockHzFromProfiles(tid, module,
                                               { HocClkProfile_HandheldChargingUSB, HocClkProfile_HandheldCharging, HocClkProfile_Handheld },
                                               returnRaw ? 1 : 1000000);
            case HocClkProfile_HandheldChargingOfficial:
                return FindClockHzFromProfiles(tid, module,
                                               { HocClkProfile_HandheldChargingOfficial, HocClkProfile_HandheldCharging, HocClkProfile_Handheld },
                                               returnRaw ? 1 : 1000000);
            case HocClkProfile_Docked:
                return FindClockHzFromProfiles(tid, module, { HocClkProfile_Docked }, returnRaw ? 1 : 1000000);
            default:
                ERROR_THROW("Unhandled HocClkProfile: %u", profile);
        }
        return 0;
    }

    void GetProfiles(std::uint64_t tid, HocClkTitleProfileList *out_profiles) {
        std::scoped_lock lock{ gConfigMutex };
        LoadProfileTid(tid);
        for (unsigned int profile = 0; profile < HocClkProfile_EnumMax; profile++) {
            for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                out_profiles->mhzMap[profile][module] = FindClockMHz(tid, (HocClkModule)module, (HocClkProfile)profile);
            }
        }
    }

    bool SetProfiles(std::uint64_t tid, HocClkTitleProfileList *profiles, bool immediate) {
        std::scoped_lock lock{ gConfigMutex };

        mkdir(FILE_PROFILES_DIR, 0777);

        std::string profilePath = ProfilePathForTid(tid);

        uint8_t numProfiles = 0;

        std::vector<std::string> keys;
        std::vector<std::string> values;
        keys.reserve(+HocClkProfile_EnumMax * +HocClkModule_EnumMax);
        values.reserve(+HocClkProfile_EnumMax * +HocClkModule_EnumMax);

        std::uint32_t *mhz = &profiles->mhz[0];

        for (unsigned int profile = 0; profile < HocClkProfile_EnumMax; profile++) {
            for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                if (*mhz) {
                    numProfiles++;

                    std::string key =
                        std::string(board::GetProfileName((HocClkProfile)profile, false)) + "_" + board::GetModuleName((HocClkModule)module, false);
                    std::string value = std::to_string(*mhz);

                    keys.push_back(key);
                    values.push_back(value);
                }
                mhz++;
            }
        }

        std::vector<const char *> keyPointers;
        std::vector<const char *> valuePointers;
        keyPointers.reserve(keys.size() + 1);
        valuePointers.reserve(values.size() + 1);

        for (size_t i = 0; i < keys.size(); i++) {
            keyPointers.push_back(keys[i].c_str());
            valuePointers.push_back(values[i].c_str());
        }
        keyPointers.push_back(NULL);
        valuePointers.push_back(NULL);

        if (keys.empty()) {
            struct stat st;
            if (stat(profilePath.c_str(), &st) == 0 && remove(profilePath.c_str()) != 0) {
                fileUtils::LogLine("[cfg] Failed to remove empty profile %s", profilePath.c_str());
                return false;
            }
        } else if (!ini_putsection(CONFIG_VAL_SECTION, keyPointers.data(), valuePointers.data(), profilePath.c_str())) {
            return false;
        }

        gProfileMtimes[tid] = CheckFileMtime(profilePath);

        if (immediate) {
            mhz = &profiles->mhz[0];
            gProfileCountMap[tid] = numProfiles;
            for (unsigned int profile = 0; profile < HocClkProfile_EnumMax; profile++) {
                for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                    if (*mhz) {
                        gProfileMHzMap[std::make_tuple(tid, (HocClkProfile)profile, (HocClkModule)module)] = *mhz;
                    } else {
                        gProfileMHzMap.erase(std::make_tuple(tid, (HocClkProfile)profile, (HocClkModule)module));
                    }
                    mhz++;
                }
            }
            gLoadedTids.insert(tid);
            if (tid != HOCCLK_GLOBAL_PROFILE_TID) {
                gCurrentGameTid = tid;
            }

            SetProfileDirty(true);
        }

        return true;
    }

    std::uint8_t GetProfileCount(std::uint64_t tid) {
        std::scoped_lock lock{ gConfigMutex };
        if (tid != 0 && gLoaded && !gLoadedTids.count(tid)) {
            LoadProfileTid(tid);
        }
        auto it = gProfileCountMap.find(tid);
        if (it == gProfileCountMap.end()) {
            return 0;
        }
        return it->second;
    }

    void SetEnabled(bool enabled) {
        gEnabled = enabled;
    }

    bool Enabled() {
        return gEnabled;
    }

    bool MigrationHappened() {
        return gMigrationHappened;
    }

    void SetOverrideHz(HocClkModule module, std::uint32_t hz) {
        ASSERT_ENUM_VALID(HocClkModule, module);
        std::scoped_lock lock{ gOverrideMutex };
        gOverrideFreqs[module] = hz;
    }

    std::uint32_t GetOverrideHz(HocClkModule module) {
        ASSERT_ENUM_VALID(HocClkModule, module);
        std::scoped_lock lock{ gOverrideMutex };
        return gOverrideFreqs[module];
    }

    std::uint64_t GetConfigValue(HocClkConfigValue kval) {
        ASSERT_ENUM_VALID(HocClkConfigValue, kval);
        std::scoped_lock lock{ gConfigMutex };
        return configValues[kval];
    }

    const char *GetConfigValueName(HocClkConfigValue kval, bool pretty) {
        ASSERT_ENUM_VALID(HocClkConfigValue, kval);
        return hocclkFormatConfigValue(kval, pretty);
    }

    void GetConfigValues(HocClkConfigValueList *out_configValues) {
        std::scoped_lock lock{ gConfigMutex };
        for (unsigned int kval = 0; kval < HocClkConfigValue_EnumMax; kval++) {
            out_configValues->values[kval] = configValues[kval];
        }
    }

    bool SetConfigValues(HocClkConfigValueList *configValues, bool immediate) {
        std::scoped_lock lock{ gConfigMutex };

        std::vector<std::string> settingsKeys;
        std::vector<std::string> settingsValues;
        std::vector<std::string> kipKeys;
        std::vector<std::string> kipValues;

        for (unsigned int kval = 0; kval < HocClkConfigValue_EnumMax; kval++) {
            if (!hocclkValidConfigValue((HocClkConfigValue)kval, configValues->values[kval]) ||
                configValues->values[kval] == hocclkDefaultConfigValue((HocClkConfigValue)kval)) {
                continue;
            }
            std::string key = hocclkFormatConfigValue((HocClkConfigValue)kval, false);
            std::string value = std::to_string(configValues->values[kval]);

            if (hocclkIsKipConfigValue((HocClkConfigValue)kval)) {
                kipKeys.push_back(key);
                kipValues.push_back(value);
            } else {
                settingsKeys.push_back(key);
                settingsValues.push_back(value);
            }
        }

        bool success = true;

        if (!settingsKeys.empty()) {
            std::vector<const char *> sKeyPtrs;
            std::vector<const char *> sValPtrs;
            sKeyPtrs.reserve(settingsKeys.size() + 1);
            sValPtrs.reserve(settingsValues.size() + 1);
            for (size_t i = 0; i < settingsKeys.size(); i++) {
                sKeyPtrs.push_back(settingsKeys[i].c_str());
                sValPtrs.push_back(settingsValues[i].c_str());
            }
            sKeyPtrs.push_back(NULL);
            sValPtrs.push_back(NULL);
            if (!ini_putsection(CONFIG_VAL_SECTION, sKeyPtrs.data(), sValPtrs.data(), gSettingsPath.c_str())) {
                success = false;
            }
        }

        if (!kipKeys.empty()) {
            std::vector<const char *> kKeyPtrs;
            std::vector<const char *> kValPtrs;
            kKeyPtrs.reserve(kipKeys.size() + 1);
            kValPtrs.reserve(kipValues.size() + 1);
            for (size_t i = 0; i < kipKeys.size(); i++) {
                kKeyPtrs.push_back(kipKeys[i].c_str());
                kValPtrs.push_back(kipValues[i].c_str());
            }
            kKeyPtrs.push_back(NULL);
            kValPtrs.push_back(NULL);
            if (!ini_putsection(CONFIG_VAL_SECTION, kKeyPtrs.data(), kValPtrs.data(), gKipPath.c_str())) {
                success = false;
            }
        }

        if (immediate) {
            for (unsigned int kval = 0; kval < HocClkConfigValue_EnumMax; kval++) {
                if (hocclkValidConfigValue((HocClkConfigValue)kval, configValues->values[kval])) {
                    config::configValues[kval] = configValues->values[kval];
                } else {
                    config::configValues[kval] = hocclkDefaultConfigValue((HocClkConfigValue)kval);
                }
            }
            SetProfileDirty(true);
        }

        return success;
    }

    bool ResetConfigValue(HocClkConfigValue kval) {
        if (!HOCCLK_ENUM_VALID(HocClkConfigValue, kval)) {
            fileUtils::LogLine("[cfg] Invalid HocClkConfigValue: %u", kval);
            return false;
        }

        std::scoped_lock lock{ gConfigMutex };

        std::uint64_t defaultValue = hocclkDefaultConfigValue(kval);

        std::vector<const char *> iniKeys;
        std::vector<std::string> iniValues;
        iniKeys.reserve(2);
        iniValues.reserve(1);

        iniKeys.push_back(hocclkFormatConfigValue(kval, false));
        iniValues.push_back("");
        iniKeys.push_back(NULL);

        std::vector<const char *> valuePointers;
        valuePointers.reserve(iniValues.size() + 1);
        for (const auto &val : iniValues) {
            valuePointers.push_back(val.c_str());
        }
        valuePointers.push_back(NULL);

        const char *targetPath = hocclkIsKipConfigValue(kval) ? gKipPath.c_str() : gSettingsPath.c_str();

        if (!ini_putsection(CONFIG_VAL_SECTION, iniKeys.data(), valuePointers.data(), targetPath)) {
            fileUtils::LogLine("[cfg] Failed to reset config value %u in INI", kval);
            return false;
        }

        configValues[kval] = defaultValue;
        fileUtils::LogLine("[cfg] Reset config value %u to default: %llu", kval, defaultValue);
        SetProfileDirty(true);

        return true;
    }

    bool SetConfigValue(HocClkConfigValue kval, std::uint64_t value, bool immediate) {
        if (!HOCCLK_ENUM_VALID(HocClkConfigValue, kval)) {
            return false;
        }
        if (!hocclkValidConfigValue(kval, value)) {
            return false;
        }

        std::scoped_lock lock{ gConfigMutex };

        std::vector<const char *> iniKeys;
        std::vector<std::string> iniValues;
        iniKeys.reserve(2);
        iniValues.reserve(1);

        iniKeys.push_back(hocclkFormatConfigValue(kval, false));
        iniValues.push_back(std::to_string(value));
        iniKeys.push_back(NULL);

        std::vector<const char *> valuePointers;
        valuePointers.reserve(2);
        valuePointers.push_back(iniValues[0].c_str());
        valuePointers.push_back(NULL);

        const char *targetPath = hocclkIsKipConfigValue(kval) ? gKipPath.c_str() : gSettingsPath.c_str();

        if (!ini_putsection(CONFIG_VAL_SECTION, iniKeys.data(), valuePointers.data(), targetPath)) {
            return false;
        }

        if (immediate) {
            configValues[kval] = value;
            SetProfileDirty(true);
        }

        return true;
    }

    void DeleteKey(const char *section, const char *key) {
        std::scoped_lock lock{ gConfigMutex };
        ini_puts(section, key, NULL, gSettingsPath.c_str());
        ini_puts(section, key, NULL, gKipPath.c_str());
    }
}  // namespace config
