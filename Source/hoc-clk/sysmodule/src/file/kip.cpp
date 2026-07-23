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

#include "../board/board.hpp"
#include "../i2c/i2cDrv.h"
#include "../mgr/clock_manager.hpp"
#include "file_utils.hpp"
#include "../mapping/mem_map.hpp"
#include "kip.hpp"

namespace kip {

    bool kipAvailable = false;
    void SetKipData() {
        // TODO: figure out if this REALLY causes issues (i doubt it)
        // if(board::GetSocType() == HocClkSocType_Mariko) {
        //     if(R_FAILED(I2c_BuckConverter_SetMvOut(&I2c_Mariko_DRAM_VDDQ, config::GetConfigValue(KipConfigValue_marikoEmcVddqVolt) / 1000))) {
        //         fileUtils::LogLine("[clock_manager] Failed set i2c vddq");
        //         notification::writeNotification("Horizon OC\nFailed to write I2C\nwhile setting vddq");
        //     }
        // }
        CustomizeTable table;
        FILE *fp = fopen("sdmc:/atmosphere/kips/hoc.kip", "r+b");

        if (fp == NULL) {
            notification::writeNotification("Horizon OC\nKip opening failed");
            kipAvailable = false;
            return;
        }
        kipAvailable = true;

        if (!cust_read_table_f(fp, &table)) {
            fclose(fp);
            fileUtils::LogLine("[kip] Failed to read KIP file");
            notification::writeNotification("Horizon OC\nKip read failed");
            return;
        }

        u32 custRev = cust_get_cust_rev(&table);
        u32 kipVersion = cust_get_kip_version(&table);
        if (custRev < CUST_REV || kipVersion < KIP_VERSION) {
            fclose(fp);
            notification::writeNotification("Horizon OC\nOutdated kip detected!\nPlease update Horizon OC");
            fileUtils::LogLine("Cust revision: %u", custRev);
            fileUtils::LogLine("Kip version: %u", kipVersion);
            return;
        } else if (custRev > CUST_REV || kipVersion > KIP_VERSION) {
            fclose(fp);
            notification::writeNotification("Horizon OC\nOutdated sysmodule detected!\nPlease update Horizon OC");
            fileUtils::LogLine("Cust revision: %u", custRev);
            fileUtils::LogLine("Kip version: %u", kipVersion);
            return;
        }

        // CUST_WRITE_FIELD_BATCH(&table, mtcConf, config::GetConfigValue(KipConfigValue_mtcConf));
        CUST_WRITE_FIELD_BATCH(&table, hpMode, config::GetConfigValue(KipConfigValue_hpMode));

        CUST_WRITE_FIELD_BATCH(&table, commonEmcMemVolt, config::GetConfigValue(KipConfigValue_commonEmcMemVolt));
        CUST_WRITE_FIELD_BATCH(&table, eristaEmcMaxClock, config::GetConfigValue(KipConfigValue_eristaEmcMaxClock));
        CUST_WRITE_FIELD_BATCH(&table, marikoEmcMaxClock, config::GetConfigValue(KipConfigValue_marikoEmcMaxClock));
        CUST_WRITE_FIELD_BATCH(&table, marikoEmcVddqVolt, config::GetConfigValue(KipConfigValue_marikoEmcVddqVolt));
        CUST_WRITE_FIELD_BATCH(&table, emcDvbShift, config::GetConfigValue(KipConfigValue_emcDvbShift));
        CUST_WRITE_FIELD_BATCH(&table, marikoSocVmax, config::GetConfigValue(KipConfigValue_marikoSocVmax));

        CUST_WRITE_FIELD_BATCH(&table, t1_tRCD, config::GetConfigValue(KipConfigValue_t1_tRCD));
        CUST_WRITE_FIELD_BATCH(&table, t2_tRP, config::GetConfigValue(KipConfigValue_t2_tRP));
        CUST_WRITE_FIELD_BATCH(&table, t3_tRAS, config::GetConfigValue(KipConfigValue_t3_tRAS));
        CUST_WRITE_FIELD_BATCH(&table, t4_tRRD, config::GetConfigValue(KipConfigValue_t4_tRRD));
        CUST_WRITE_FIELD_BATCH(&table, t5_tRFC, config::GetConfigValue(KipConfigValue_t5_tRFC));
        CUST_WRITE_FIELD_BATCH(&table, t6_tRTW, config::GetConfigValue(KipConfigValue_t6_tRTW));
        CUST_WRITE_FIELD_BATCH(&table, t7_tWTR, config::GetConfigValue(KipConfigValue_t7_tWTR));
        CUST_WRITE_FIELD_BATCH(&table, t8_tREFI, config::GetConfigValue(KipConfigValue_t8_tREFI));
        CUST_WRITE_FIELD_BATCH(&table, stepMode, config::GetConfigValue(KipConfigValue_stepMode));

        CUST_WRITE_FIELD_BATCH(&table, timingEmcTbreak, config::GetConfigValue(KipConfigValue_timingEmcTbreak));
        CUST_WRITE_FIELD_BATCH(&table, low_t1_tRCD, config::GetConfigValue(KipConfigValue_low_t1_tRCD));
        CUST_WRITE_FIELD_BATCH(&table, low_t2_tRP, config::GetConfigValue(KipConfigValue_low_t2_tRP));
        CUST_WRITE_FIELD_BATCH(&table, low_t3_tRAS, config::GetConfigValue(KipConfigValue_low_t3_tRAS));
        CUST_WRITE_FIELD_BATCH(&table, low_t4_tRRD, config::GetConfigValue(KipConfigValue_low_t4_tRRD));
        CUST_WRITE_FIELD_BATCH(&table, low_t5_tRFC, config::GetConfigValue(KipConfigValue_low_t5_tRFC));
        CUST_WRITE_FIELD_BATCH(&table, low_t6_tRTW, config::GetConfigValue(KipConfigValue_low_t6_tRTW));
        CUST_WRITE_FIELD_BATCH(&table, low_t7_tWTR, config::GetConfigValue(KipConfigValue_low_t7_tWTR));
        CUST_WRITE_FIELD_BATCH(&table, low_t8_tREFI, config::GetConfigValue(KipConfigValue_low_t8_tREFI));

        CUST_WRITE_FIELD_BATCH(&table, readLatency1333, config::GetConfigValue(KipConfigValue_read_latency_1333));
        CUST_WRITE_FIELD_BATCH(&table, readLatency1600, config::GetConfigValue(KipConfigValue_read_latency_1600));
        CUST_WRITE_FIELD_BATCH(&table, readLatency1866, config::GetConfigValue(KipConfigValue_read_latency_1866));
        CUST_WRITE_FIELD_BATCH(&table, readLatency2133, config::GetConfigValue(KipConfigValue_read_latency_2133));

        CUST_WRITE_FIELD_BATCH(&table, writeLatency1333, config::GetConfigValue(KipConfigValue_write_latency_1333));
        CUST_WRITE_FIELD_BATCH(&table, writeLatency1600, config::GetConfigValue(KipConfigValue_write_latency_1600));
        CUST_WRITE_FIELD_BATCH(&table, writeLatency1866, config::GetConfigValue(KipConfigValue_write_latency_1866));
        CUST_WRITE_FIELD_BATCH(&table, writeLatency2133, config::GetConfigValue(KipConfigValue_write_latency_2133));

        CUST_WRITE_FIELD_BATCH(&table, eristaCpuUV, config::GetConfigValue(KipConfigValue_eristaCpuUV));
        CUST_WRITE_FIELD_BATCH(&table, eristaCpuVmin, config::GetConfigValue(KipConfigValue_eristaCpuVmin));
        CUST_WRITE_FIELD_BATCH(&table, eristaCpuMaxVolt, config::GetConfigValue(KipConfigValue_eristaCpuMaxVolt));
        CUST_WRITE_FIELD_BATCH(&table, eristaCpuUnlock, config::GetConfigValue(KipConfigValue_eristaCpuUnlock));

        CUST_WRITE_FIELD_BATCH(&table, marikoCpuUVLow, config::GetConfigValue(KipConfigValue_marikoCpuUVLow));
        CUST_WRITE_FIELD_BATCH(&table, marikoCpuUVHigh, config::GetConfigValue(KipConfigValue_marikoCpuUVHigh));
        CUST_WRITE_FIELD_BATCH(&table, tableConf, config::GetConfigValue(KipConfigValue_tableConf));
        CUST_WRITE_FIELD_BATCH(&table, marikoCpuLowVmin, config::GetConfigValue(KipConfigValue_marikoCpuLowVmin));
        CUST_WRITE_FIELD_BATCH(&table, marikoCpuHighVmin, config::GetConfigValue(KipConfigValue_marikoCpuHighVmin));
        CUST_WRITE_FIELD_BATCH(&table, marikoCpuMaxVolt, config::GetConfigValue(KipConfigValue_marikoCpuMaxVolt));
        CUST_WRITE_FIELD_BATCH(&table, marikoCpuMaxClock, config::GetConfigValue(KipConfigValue_marikoCpuMaxClock));

        CUST_WRITE_FIELD_BATCH(&table, eristaCpuBoostClock, config::GetConfigValue(KipConfigValue_eristaCpuBoostClock));
        CUST_WRITE_FIELD_BATCH(&table, marikoCpuBoostClock, config::GetConfigValue(KipConfigValue_marikoCpuBoostClock));

        CUST_WRITE_FIELD_BATCH(&table, eristaGpuUV, config::GetConfigValue(KipConfigValue_eristaGpuUV));
        CUST_WRITE_FIELD_BATCH(&table, eristaGpuVmin, config::GetConfigValue(KipConfigValue_eristaGpuVmin));

        CUST_WRITE_FIELD_BATCH(&table, marikoGpuUV, config::GetConfigValue(KipConfigValue_marikoGpuUV));
        CUST_WRITE_FIELD_BATCH(&table, marikoGpuVmin, config::GetConfigValue(KipConfigValue_marikoGpuVmin));
        CUST_WRITE_FIELD_BATCH(&table, marikoGpuVmax, config::GetConfigValue(KipConfigValue_marikoGpuVmax));

        CUST_WRITE_FIELD_BATCH(&table, commonGpuVoltOffset, config::GetConfigValue(KipConfigValue_commonGpuVoltOffset));

        for (int i = 0; i < 24; i++) {
            table.marikoGpuVoltArray[i] = config::GetConfigValue((HocClkConfigValue)(KipConfigValue_g_volt_76800 + i));
        }

        for (int i = 0; i < 27; i++) {
            table.eristaGpuVoltArray[i] = config::GetConfigValue((HocClkConfigValue)(KipConfigValue_g_volt_e_76800 + i));
        }

        for (size_t i = 0; i < 28; ++i) {
            table.marikoSocVoltArray[i] = config::GetConfigValue((HocClkConfigValue) (KipConfigValue_g_soc_volt_1866000 + i));
        }

        CUST_WRITE_FIELD_BATCH(&table, t6_tRTW_fine_tune, config::GetConfigValue(KipConfigValue_t6_tRTW_fine_tune));
        CUST_WRITE_FIELD_BATCH(&table, t7_tWTR_fine_tune, config::GetConfigValue(KipConfigValue_t7_tWTR_fine_tune));

        if (!cust_write_table_f(fp, &table)) {
            fclose(fp);
            fileUtils::LogLine("[kip] Failed to write KIP file");
            notification::writeNotification("Horizon OC\nKip write failed");
            return;
        }
        fclose(fp);

        HocClkConfigValueList configValues;
        config::GetConfigValues(&configValues);

        configValues.values[KipCrc32] = (u64)crc32::checksum_file("sdmc:/atmosphere/kips/hoc.kip");  // write checksum

        if (config::SetConfigValues(&configValues, true)) {
            fileUtils::LogLine("[kip] KIP data set. CRC32: %ld (Cust Rev %ld)", configValues.values[KipCrc32],
                               configValues.values[KipConfigValue_custRev]);
            for (u64 i = KipConfigValue_hpMode; i < HocClkConfigValue_EnumMax; i++) {
                fileUtils::LogLine("%s: %ld", hocclkFormatConfigValue((HocClkConfigValue)i, false), configValues.values[i]);
            }
        } else {
            fileUtils::LogLine("[kip] Warning: Failed to set config values from KIP");
            notification::writeNotification("Horizon OC\nKip config set failed");
        }
    }

    bool IsKipLoaded() {
        constexpr u32 ExpectedMagic = 0x686F634D;
        constexpr uintptr_t LoadMagicAddress = 0x4003DC00;
        u32 iramValue = {};

        SmcCopyFromIram(&iramValue, LoadMagicAddress, sizeof(iramValue));

        if (iramValue == ExpectedMagic) {
            iramValue = 0;
            SmcCopyToIram(LoadMagicAddress, &iramValue, sizeof(iramValue));
            return true;
        }

        notification::writeNotification("Kip is not loaded!");
        fileUtils::LogLine("Kip was not loaded!");
        return false;
    }

    // I know this is very hacky, but the config system in the sysmodule doesn't really support writing

    void GetKipData() {
        FILE *fp = fopen("sdmc:/atmosphere/kips/hoc.kip", "rb");

        if (fp == NULL) {
            notification::writeNotification("Horizon OC\nKip opening failed");
            kipAvailable = false;
            return;
        }
        kipAvailable = true;

        HocClkConfigValueList configValues;
        config::GetConfigValues(&configValues);

        CustomizeTable table;
        if (!cust_read_table_f(fp, &table)) {
            fclose(fp);
            fileUtils::LogLine("[kip] Failed to read KIP file for GetKipData");
            notification::writeNotification("Horizon OC\nKip read failed");
            return;
        }
        fclose(fp);

        // if(cust_get_cust_rev(&table) != CUST_REV) {
        //     notification::writeNotification("Horizon OC\nKip version mismatch\nPlease reinstall Horizon OC");
        //     return;
        // }

        if ((u64)crc32::checksum_file("sdmc:/atmosphere/kips/hoc.kip") != config::GetConfigValue(KipCrc32) &&
            !config::GetConfigValue(HocClkConfigValue_IsFirstLoad)) {
            MigrateKipData(cust_get_cust_rev(&table), cust_get_kip_version(&table));
            SetKipData();
            notification::writeNotification("Horizon OC\nKIP has been updated\nPlease reboot your console");
            return;
        }
        if (config::GetConfigValue(HocClkConfigValue_IsFirstLoad) == true) {
            configValues.values[HocClkConfigValue_IsFirstLoad] = (u64) false;
            notification::writeNotification("Horizon OC has been installed");
        }

        configValues.values[KipCrc32] = (u64)crc32::checksum_file("sdmc:/atmosphere/kips/hoc.kip");  // write checksum
        // configValues.values[KipConfigValue_mtcConf] = cust_get_mtc_conf(&table);
        clockManager::gContext.custRev = cust_get_cust_rev(&table);

        u32 custRev = cust_get_cust_rev(&table);
        u32 kipVersion = cust_get_kip_version(&table);
        if (custRev < CUST_REV || kipVersion < KIP_VERSION) {
            notification::writeNotification("Horizon OC\nOutdated kip detected!\nPlease update Horizon OC");
            fileUtils::LogLine("Cust revision: %u", custRev);
            fileUtils::LogLine("Kip version: %u", kipVersion);
            return;
        } else if (custRev > CUST_REV || kipVersion > KIP_VERSION) {
            notification::writeNotification("Horizon OC\nOutdated sysmodule detected!\nPlease update Horizon OC");
            fileUtils::LogLine("Cust revision: %u", custRev);
            fileUtils::LogLine("Kip version: %u", kipVersion);
            return;
        }

        clockManager::gContext.isKipLoaded = IsKipLoaded();
        clockManager::gContext.kipVersion = kipVersion;
        configValues.values[KipConfigValue_custRev] = cust_get_cust_rev(&table);
        configValues.values[KipConfigValue_KipVersion] = cust_get_kip_version(&table);  // Run this after the check so we can do migration process
        configValues.values[KipConfigValue_hpMode] = cust_get_hp_mode(&table);

        configValues.values[KipConfigValue_commonEmcMemVolt] = cust_get_common_emc_volt(&table);
        configValues.values[KipConfigValue_eristaEmcMaxClock] = cust_get_erista_emc_max(&table);
        configValues.values[KipConfigValue_marikoEmcMaxClock] = cust_get_mariko_emc_max(&table);
        configValues.values[KipConfigValue_marikoEmcVddqVolt] = cust_get_mariko_emc_vddq(&table);
        configValues.values[KipConfigValue_emcDvbShift] = cust_get_emc_dvb_shift(&table);
        configValues.values[KipConfigValue_marikoSocVmax] = cust_get_marikoSocVmax(&table);

        configValues.values[KipConfigValue_t1_tRCD] = cust_get_tRCD(&table);
        configValues.values[KipConfigValue_t2_tRP] = cust_get_tRP(&table);
        configValues.values[KipConfigValue_t3_tRAS] = cust_get_tRAS(&table);
        configValues.values[KipConfigValue_t4_tRRD] = cust_get_tRRD(&table);
        configValues.values[KipConfigValue_t5_tRFC] = cust_get_tRFC(&table);
        configValues.values[KipConfigValue_t6_tRTW] = cust_get_tRTW(&table);
        configValues.values[KipConfigValue_t7_tWTR] = cust_get_tWTR(&table);
        configValues.values[KipConfigValue_t8_tREFI] = cust_get_tREFI(&table);
        configValues.values[KipConfigValue_stepMode] = cust_get_step_mode(&table);

        configValues.values[KipConfigValue_timingEmcTbreak] = cust_get_timing_emc_tbreak(&table);
        configValues.values[KipConfigValue_low_t1_tRCD] = cust_get_low_tRCD(&table);
        configValues.values[KipConfigValue_low_t2_tRP] = cust_get_low_tRP(&table);
        configValues.values[KipConfigValue_low_t3_tRAS] = cust_get_low_tRAS(&table);
        configValues.values[KipConfigValue_low_t4_tRRD] = cust_get_low_tRRD(&table);
        configValues.values[KipConfigValue_low_t5_tRFC] = cust_get_low_tRFC(&table);
        configValues.values[KipConfigValue_low_t6_tRTW] = cust_get_low_tRTW(&table);
        configValues.values[KipConfigValue_low_t7_tWTR] = cust_get_low_tWTR(&table);
        configValues.values[KipConfigValue_low_t8_tREFI] = cust_get_low_tREFI(&table);

        configValues.values[KipConfigValue_read_latency_1333] = cust_get_read_latency_1333(&table);
        configValues.values[KipConfigValue_read_latency_1600] = cust_get_read_latency_1600(&table);
        configValues.values[KipConfigValue_read_latency_1866] = cust_get_read_latency_1866(&table);
        configValues.values[KipConfigValue_read_latency_2133] = cust_get_read_latency_2133(&table);

        configValues.values[KipConfigValue_write_latency_1333] = cust_get_write_latency_1333(&table);
        configValues.values[KipConfigValue_write_latency_1600] = cust_get_write_latency_1600(&table);
        configValues.values[KipConfigValue_write_latency_1866] = cust_get_write_latency_1866(&table);
        configValues.values[KipConfigValue_write_latency_2133] = cust_get_write_latency_2133(&table);

        configValues.values[KipConfigValue_eristaCpuUV] = cust_get_erista_cpu_uv(&table);
        configValues.values[KipConfigValue_eristaCpuVmin] = cust_get_eristaCpuVmin(&table);
        configValues.values[KipConfigValue_eristaCpuMaxVolt] = cust_get_erista_cpu_max_volt(&table);
        configValues.values[KipConfigValue_eristaCpuUnlock] = cust_get_eristaCpuUnlock(&table);

        configValues.values[KipConfigValue_marikoCpuUVLow] = cust_get_mariko_cpu_uv_low(&table);
        configValues.values[KipConfigValue_marikoCpuUVHigh] = cust_get_mariko_cpu_uv_high(&table);
        configValues.values[KipConfigValue_tableConf] = cust_get_table_conf(&table);
        configValues.values[KipConfigValue_marikoCpuLowVmin] = cust_get_mariko_cpu_low_vmin(&table);
        configValues.values[KipConfigValue_marikoCpuHighVmin] = cust_get_mariko_cpu_high_vmin(&table);
        configValues.values[KipConfigValue_marikoCpuMaxVolt] = cust_get_mariko_cpu_max_volt(&table);
        configValues.values[KipConfigValue_marikoCpuMaxClock] = cust_get_marikoCpuMaxClock(&table);
        configValues.values[KipConfigValue_eristaCpuBoostClock] = cust_get_erista_cpu_boost(&table);
        configValues.values[KipConfigValue_marikoCpuBoostClock] = cust_get_mariko_cpu_boost(&table);

        configValues.values[KipConfigValue_eristaGpuUV] = cust_get_erista_gpu_uv(&table);
        configValues.values[KipConfigValue_eristaGpuVmin] = cust_get_erista_gpu_vmin(&table);
        configValues.values[KipConfigValue_marikoGpuUV] = cust_get_mariko_gpu_uv(&table);
        configValues.values[KipConfigValue_marikoGpuVmin] = cust_get_mariko_gpu_vmin(&table);
        configValues.values[KipConfigValue_marikoGpuVmax] = cust_get_mariko_gpu_vmax(&table);
        configValues.values[KipConfigValue_commonGpuVoltOffset] = cust_get_common_gpu_offset(&table);

        for (int i = 0; i < 24; i++) {
            configValues.values[KipConfigValue_g_volt_76800 + i] = cust_get_mariko_gpu_volt(&table, i);
        }

        for (int i = 0; i < 27; i++) {
            configValues.values[KipConfigValue_g_volt_e_76800 + i] = cust_get_erista_gpu_volt(&table, i);
        }

        for (size_t i = 0; i < 26; ++i) {
            configValues.values[KipConfigValue_g_soc_volt_1866000 + i] = cust_get_mariko_soc_volt(&table, i);
        }

        configValues.values[KipConfigValue_t7_tWTR_fine_tune] = cust_get_tWTR_fine_tune(&table);
        configValues.values[KipConfigValue_t6_tRTW_fine_tune] = cust_get_tRTW_fine_tune(&table);

        if (sizeof(HocClkConfigValueList) <= sizeof(configValues)) {
            if (config::SetConfigValues(&configValues, true)) {
                fileUtils::LogLine("[kip] KIP loaded. CRC32: %ld (Cust Rev %ld)", configValues.values[KipCrc32],
                                   configValues.values[KipConfigValue_custRev]);
                for (u64 i = KipConfigValue_hpMode; i < HocClkConfigValue_EnumMax; i++) {
                    fileUtils::LogLine("%s: %ld", hocclkFormatConfigValue((HocClkConfigValue)i, false), configValues.values[i]);
                }
            } else {
                fileUtils::LogLine("[kip] Warning: Failed to set config values from KIP");
                notification::writeNotification("Horizon OC\nKip config set failed");
            }
        } else {
            fileUtils::LogLine("[kip] Error: Config value list buffer size mismatch");
            notification::writeNotification("Horizon OC\nConfig Buffer Mismatch");
        }
    }

    void MigrateKipData(u32 custRev, u32 version) {
        HocClkConfigValueList configValues;
        config::GetConfigValues(&configValues);
        u32 previousVersion = configValues.values[KipConfigValue_KipVersion];
        if (previousVersion < 240 && version >= 240) {
            // <2.4.0 -> 2.4.0 migration

            configValues.values[KipConfigValue_marikoGpuUV] += 2;  // Raise UV levels
            configValues.values[KipConfigValue_commonGpuVoltOffset] =
                (u32)(-(s64)(configValues.values[KipConfigValue_commonGpuVoltOffset]));  // Migrate GPU Volt Offset
            // Raise min cpu vmin
            if (configValues.values[KipConfigValue_eristaCpuVmin] < 750) {
                configValues.values[KipConfigValue_eristaCpuVmin] = 750;
            }

            // delete handheld TDP config entries
            config::DeleteKey(CONFIG_VAL_SECTION, "handheld_tdp");
            config::DeleteKey(CONFIG_VAL_SECTION, "tdp_limit");
            config::DeleteKey(CONFIG_VAL_SECTION, "tdp_limit_l");
        }
        config::SetConfigValues(&configValues, true);
    }

}  // namespace kip
