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

#pragma once

#include <set>
#include <vector>
#include <hocclk.h>
#include <switch.h>

#define CONFIG_VAL_SECTION "values"

namespace config {

    void Initialize();
    void Exit();

    bool Refresh();
    bool HasProfilesLoaded();

    bool IsProfileDirty();
    void SetProfileDirty(bool dirty);

    std::uint8_t GetProfileCount(std::uint64_t tid);
    void GetProfiles(std::uint64_t tid, HocClkTitleProfileList *out_profiles);
    bool SetProfiles(std::uint64_t tid, HocClkTitleProfileList *profiles, bool immediate);
    std::uint32_t GetAutoClockHz(std::uint64_t tid, HocClkModule module, HocClkProfile profile, bool returnRaw);

    void SetEnabled(bool enabled);
    bool Enabled();
    bool MigrationHappened();
    void SetOverrideHz(HocClkModule module, std::uint32_t hz);
    std::uint32_t GetOverrideHz(HocClkModule module);

    std::uint64_t GetConfigValue(HocClkConfigValue val);
    const char *GetConfigValueName(HocClkConfigValue val, bool pretty);
    void GetConfigValues(HocClkConfigValueList *out_configValues);
    bool SetConfigValues(HocClkConfigValueList *configValues, bool immediate);
    bool ResetConfigValue(HocClkConfigValue kval);
    bool SetConfigValue(HocClkConfigValue kval, std::uint64_t value, bool immediate = true);
    void DeleteKey(const char *section, const char *key);

    extern uint64_t configValues[HocClkConfigValue_EnumMax];

}  // namespace config
