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

#pragma once
#include <crc32.h>
#include <notification.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <switch.h>

#include "config.hpp"
#include "file_utils.hpp"


namespace kip {
    extern bool kipAvailable;

    typedef struct {
        u8 cust[4];
        u32 custRev;
        u32 kipVersion;
        u32 hpMode;
        u32 commonEmcMemVolt;
        u32 eristaEmcMaxClock;
        u32 stepMode;
        u32 marikoEmcMaxClock;
        u32 marikoEmcVddqVolt;
        u32 emcDvbShift;
        u32 marikoSocVmax;

        // advanced config
        u32 t1_tRCD;
        u32 t2_tRP;
        u32 t3_tRAS;
        u32 t4_tRRD;
        u32 t5_tRFC;
        u32 t6_tRTW;
        u32 t7_tWTR;
        u32 t8_tREFI;

        u32 timingEmcTbreak;
        u32 low_t1_tRCD;
        u32 low_t2_tRP;
        u32 low_t3_tRAS;
        u32 low_t4_tRRD;
        u32 low_t5_tRFC;
        u32 low_t6_tRTW;
        u32 low_t7_tWTR;
        u32 low_t8_tREFI;

        /* These latencies are arrays in loader, but it's easier to handle it this way in the configurator. */
        u32 readLatency1333, readLatency1600, readLatency1866, readLatency2133;
        u32 writeLatency1333, writeLatency1600, writeLatency1866, writeLatency2133;

        u32 eristaCpuUV;
        u32 eristaCpuVmin;
        u32 eristaCpuMaxVolt;
        u32 eristaCpuUnlock;

        u32 marikoCpuUVLow;
        u32 marikoCpuUVHigh;
        u32 tableConf;
        u32 marikoCpuLowVmin;
        u32 marikoCpuHighVmin;
        u32 marikoCpuMaxVolt;
        u32 marikoCpuMaxClock;

        u32 eristaCpuBoostClock;
        u32 marikoCpuBoostClock;

        u32 eristaGpuUV;
        u32 eristaGpuVmin;

        u32 marikoGpuUV;
        u32 marikoGpuVmin;
        u32 marikoGpuVmax;

        u32 commonGpuVoltOffset;

        u32 eristaGpuVoltArray[27];
        u32 marikoGpuVoltArray[25];
        s32 marikoSocVoltArray[28];

        u32 t6_tRTW_fine_tune;
        u32 t7_tWTR_fine_tune;
        
        u32 pcvLogVerbosity;

        u32 reserved[57];
    } CustomizeTable;

#define CUST_MAGIC "CUST"
#define CUST_MAGIC_LEN 4

    typedef struct {
        FILE *file;
        long offset;
        CustomizeTable cached_table;
        bool has_cache;
    } CustHandle;

    static inline bool cust_find_offset(FILE *f, long *out_offset) {
        u8 buf[512];
        long pos = 0;
        fseek(f, 0, SEEK_SET);

        while (1) {
            size_t r = fread(buf, 1, sizeof(buf), f);
            if (r < CUST_MAGIC_LEN)
                break;

            for (size_t i = 0; i <= r - CUST_MAGIC_LEN; i++) {
                if (memcmp(&buf[i], CUST_MAGIC, CUST_MAGIC_LEN) == 0) {
                    *out_offset = pos + (long)i;
                    return true;
                }
            }
            pos += (long)(r - (CUST_MAGIC_LEN - 1));
            fseek(f, pos, SEEK_SET);
        }
        return false;
    }

    static long cachedCustOffset = -1;

    static inline long cust_get_offset(FILE *f) {
        if (cachedCustOffset >= 0)
            return cachedCustOffset;
        long off;
        if (!cust_find_offset(f, &off))
            return -1;
        cachedCustOffset = off;
        return off;
    }

    static inline bool cust_read_table_f(FILE *f, CustomizeTable *out) {
        long off = cust_get_offset(f);
        if (off < 0)
            return false;

        fseek(f, 0, SEEK_END);
        if (off + (long)sizeof(CustomizeTable) > ftell(f))
            return false;

        fseek(f, off, SEEK_SET);
        if (fread(out, 1, sizeof(CustomizeTable), f) != sizeof(CustomizeTable))
            return false;

        if (memcmp(out->cust, CUST_MAGIC, CUST_MAGIC_LEN) != 0) {
            cachedCustOffset = -1;
            return false;
        }
        return true;
    }

    static inline bool cust_write_table_f(FILE *f, const CustomizeTable *in) {
        long off = cust_get_offset(f);
        if (off < 0)
            return false;

        fseek(f, 0, SEEK_END);
        if (off + (long)sizeof(CustomizeTable) > ftell(f))
            return false;

        fseek(f, off, SEEK_SET);
        bool ok = fwrite(in, 1, sizeof(CustomizeTable), f) == sizeof(CustomizeTable);
        fflush(f);
        return ok;
    }

    static inline bool cust_read_table(const char *path, CustomizeTable *out) {
        FILE *f = fopen(path, "rb");
        if (!f)
            return false;
        bool ok = cust_read_table_f(f, out);
        fclose(f);
        return ok;
    }

    static inline bool cust_write_table(const char *path, const CustomizeTable *in) {
        FILE *f = fopen(path, "r+b");
        if (!f)
            return false;
        bool ok = cust_write_table_f(f, in);
        fclose(f);
        return ok;
    }

    static inline bool cust_read_and_cache(const char *path, CustomizeTable *out) {
        return cust_read_table(path, out);
    }

#define CUST_WRITE_FIELD_BATCH(table, field, val) \
    do {                                          \
        (table)->field = (val);                   \
    } while (0)

#define CUST_WRITE_FIELD(path, field, val) \
    do {                                   \
        CustomizeTable t;                  \
        if (!cust_read_table(path, &t))    \
            return false;                  \
        t.field = (val);                   \
        return cust_write_table(path, &t); \
    } while (0)

    // static inline bool cust_set_cust_rev(const char* p, u32 v) { CUST_WRITE_FIELD(p, custRev, v); }
    // static inline bool cust_set_mtc_conf(const char* p, u32 v) { CUST_WRITE_FIELD(p, mtcConf, v); }
    static inline bool cust_set_hp_mode(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, hpMode, v);
    }

    static inline bool cust_set_common_emc_volt(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, commonEmcMemVolt, v);
    }
    static inline bool cust_set_erista_emc_max(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaEmcMaxClock, v);
    }
    static inline bool cust_set_step_mode(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, stepMode, v);
    }
    static inline bool cust_set_mariko_emc_max(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoEmcMaxClock, v);
    }
    static inline bool cust_set_mariko_emc_vddq(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoEmcVddqVolt, v);
    }
    static inline bool cust_set_emc_dvb_shift(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, emcDvbShift, v);
    }

    static inline bool cust_set_tRCD(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t1_tRCD, v);
    }
    static inline bool cust_set_tRP(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t2_tRP, v);
    }
    static inline bool cust_set_tRAS(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t3_tRAS, v);
    }
    static inline bool cust_set_tRRD(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t4_tRRD, v);
    }
    static inline bool cust_set_tRFC(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t5_tRFC, v);
    }
    static inline bool cust_set_tRTW(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t6_tRTW, v);
    }
    static inline bool cust_set_tWTR(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t7_tWTR, v);
    }
    static inline bool cust_set_tREFI(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t8_tREFI, v);
    }
    static inline bool cust_set_timing_emc_tbreak(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, timingEmcTbreak, v);
    }

    static inline bool cust_set_low_tRCD(const char *p, u32 v) { CUST_WRITE_FIELD(p, low_t1_tRCD, v); }
    static inline bool cust_set_low_tRP(const char *p, u32 v) { CUST_WRITE_FIELD(p, low_t2_tRP, v); }
    static inline bool cust_set_low_tRAS(const char *p, u32 v) { CUST_WRITE_FIELD(p, low_t3_tRAS, v); }
    static inline bool cust_set_low_tRRD(const char *p, u32 v) { CUST_WRITE_FIELD(p, low_t4_tRRD, v); }
    static inline bool cust_set_low_tRFC(const char *p, u32 v) { CUST_WRITE_FIELD(p, low_t5_tRFC, v); }

    static inline bool cust_set_low_tRTW(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, low_t6_tRTW, v);
    }
    static inline bool cust_set_low_tWTR(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, low_t7_tWTR, v);
    }

    static inline bool cust_set_low_tREFI(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, low_t8_tREFI, v);
    }

    static inline bool cust_set_tRTW_fine_tune(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t6_tRTW_fine_tune, v);
    }
    static inline bool cust_set_tWTR_fine_tune(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, t7_tWTR_fine_tune, v);
    }

    static inline bool cust_set_read_latency_1333(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, readLatency1333, v);
    }
    static inline bool cust_set_read_latency_1600(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, readLatency1600, v);
    }
    static inline bool cust_set_read_latency_1866(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, readLatency1866, v);
    }
    static inline bool cust_set_read_latency_2133(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, readLatency2133, v);
    }

    static inline bool cust_set_write_latency_1333(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, writeLatency1333, v);
    }
    static inline bool cust_set_write_latency_1600(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, writeLatency1600, v);
    }
    static inline bool cust_set_write_latency_1866(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, writeLatency1866, v);
    }
    static inline bool cust_set_write_latency_2133(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, writeLatency2133, v);
    }

    static inline bool cust_set_erista_cpu_uv(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaCpuUV, v);
    }
    static inline bool cust_set_eristaCpuVmin(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaCpuVmin, v);
    }
    static inline bool cust_set_erista_cpu_max_volt(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaCpuMaxVolt, v);
    }
    static inline bool cust_set_eristaCpuUnlock(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaCpuUnlock, v);
    }

    static inline bool cust_set_mariko_cpu_uv_low(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoCpuUVLow, v);
    }
    static inline bool cust_set_mariko_cpu_uv_high(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoCpuUVHigh, v);
    }
    static inline bool cust_set_mariko_cpu_low_vmin(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoCpuLowVmin, v);
    }
    static inline bool cust_set_mariko_cpu_high_vmin(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoCpuHighVmin, v);
    }
    static inline bool cust_set_mariko_cpu_max_volt(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoCpuMaxVolt, v);
    }
    static inline bool cust_set_erista_cpu_boost(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaCpuBoostClock, v);
    }
    static inline bool cust_set_mariko_cpu_boost(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoCpuBoostClock, v);
    }

    static inline bool cust_set_erista_gpu_uv(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaGpuUV, v);
    }
    static inline bool cust_set_erista_gpu_vmin(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, eristaGpuVmin, v);
    }
    static inline bool cust_set_mariko_gpu_uv(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoGpuUV, v);
    }
    static inline bool cust_set_mariko_gpu_vmin(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoGpuVmin, v);
    }
    static inline bool cust_set_mariko_gpu_vmax(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoGpuVmax, v);
    }
    static inline bool cust_set_common_gpu_offset(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, commonGpuVoltOffset, v);
    }
    static inline bool cust_set_marikoCpuMaxClock(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoCpuMaxClock, v);
    }
    static inline bool cust_set_marikoSocVmax(const char *p, u32 v) {
        CUST_WRITE_FIELD(p, marikoSocVmax, v);
    }

    /* GPU VOLT ARRAY HELPERS */
    static inline bool cust_set_erista_gpu_volt(const char *p, int idx, u32 v) {
        if (idx < 0 || idx >= 27)
            return false;
        CustomizeTable t;
        if (!cust_read_table(p, &t))
            return false;
        t.eristaGpuVoltArray[idx] = v;
        return cust_write_table(p, &t);
    }

    static inline bool cust_set_mariko_gpu_volt(const char *p, int idx, u32 v) {
        if (idx < 0 || idx >= 25)
            return false;
        CustomizeTable t;
        if (!cust_read_table(p, &t))
            return false;
        t.marikoGpuVoltArray[idx] = v;
        return cust_write_table(p, &t);
    }

    static inline bool cust_set_mariko_soc_volt(const char *p, int idx, u32 v) {
        if (idx < 0 || idx >= 26) {
            return false;
        }

        CustomizeTable t;
        if (!cust_read_table(p, &t)) {
            return false;
        }

        t.marikoSocVoltArray[idx] = v;

        return cust_write_table(p, &t);
    }

    static inline u32 cust_get_field(const CustomizeTable *t, u32 offset) {
        if (!t)
            return 0;
        return *(u32 *)((u8 *)t + offset);
    }

#define CUST_GET_FIELD(table, field) ((table) ? (table)->field : 0)

    static inline u32 cust_get_cust_rev(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, custRev);
    }
    static inline u32 cust_get_kip_version(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, kipVersion);
    }
    // static inline u32 cust_get_mtc_conf(const CustomizeTable* t) { return CUST_GET_FIELD(t, mtcConf); }
    static inline u32 cust_get_hp_mode(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, hpMode);
    }

    static inline u32 cust_get_common_emc_volt(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, commonEmcMemVolt);
    }
    static inline u32 cust_get_erista_emc_max(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaEmcMaxClock);
    }
    static inline u32 cust_get_step_mode(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, stepMode);
    }
    static inline u32 cust_get_mariko_emc_max(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoEmcMaxClock);
    }
    static inline u32 cust_get_mariko_emc_vddq(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoEmcVddqVolt);
    }
    static inline u32 cust_get_emc_dvb_shift(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, emcDvbShift);
    }

    static inline u32 cust_get_tRCD(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t1_tRCD);
    }
    static inline u32 cust_get_tRP(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t2_tRP);
    }
    static inline u32 cust_get_tRAS(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t3_tRAS);
    }
    static inline u32 cust_get_tRRD(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t4_tRRD);
    }
    static inline u32 cust_get_tRFC(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t5_tRFC);
    }
    static inline u32 cust_get_tRTW(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t6_tRTW);
    }
    static inline u32 cust_get_tWTR(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t7_tWTR);
    }
    static inline u32 cust_get_tREFI(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t8_tREFI);
    }
    static inline u32 cust_get_timing_emc_tbreak(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, timingEmcTbreak);
    }

    static inline u32 cust_get_low_tRCD(const CustomizeTable *t) { return CUST_GET_FIELD(t, low_t1_tRCD); }
    static inline u32 cust_get_low_tRP(const CustomizeTable *t) { return CUST_GET_FIELD(t, low_t2_tRP); }
    static inline u32 cust_get_low_tRAS(const CustomizeTable *t) { return CUST_GET_FIELD(t, low_t3_tRAS); }
    static inline u32 cust_get_low_tRRD(const CustomizeTable *t) { return CUST_GET_FIELD(t, low_t4_tRRD); }
    static inline u32 cust_get_low_tRFC(const CustomizeTable *t) { return CUST_GET_FIELD(t, low_t5_tRFC); }

    static inline u32 cust_get_low_tRTW(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, low_t6_tRTW);
    }
    static inline u32 cust_get_low_tWTR(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, low_t7_tWTR);
    }
    static inline u32 cust_get_low_tREFI(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, low_t8_tREFI);
    }
    static inline u32 cust_get_tRTW_fine_tune(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t6_tRTW_fine_tune);
    }
    static inline u32 cust_get_tWTR_fine_tune(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, t7_tWTR_fine_tune);
    }

    static inline u32 cust_get_log_verbosity(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, pcvLogVerbosity);
    }

    static inline u32 cust_get_read_latency_1333(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, readLatency1333);
    }
    static inline u32 cust_get_read_latency_1600(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, readLatency1600);
    }
    static inline u32 cust_get_read_latency_1866(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, readLatency1866);
    }
    static inline u32 cust_get_read_latency_2133(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, readLatency2133);
    }

    static inline u32 cust_get_write_latency_1333(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, writeLatency1333);
    }
    static inline u32 cust_get_write_latency_1600(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, writeLatency1600);
    }
    static inline u32 cust_get_write_latency_1866(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, writeLatency1866);
    }
    static inline u32 cust_get_write_latency_2133(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, writeLatency2133);
    }

    static inline u32 cust_get_erista_cpu_uv(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaCpuUV);
    }
    static inline u32 cust_get_eristaCpuVmin(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaCpuVmin);
    }
    static inline u32 cust_get_erista_cpu_max_volt(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaCpuMaxVolt);
    }
    static inline u32 cust_get_eristaCpuUnlock(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaCpuUnlock);
    }

    static inline u32 cust_get_mariko_cpu_uv_low(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoCpuUVLow);
    }
    static inline u32 cust_get_mariko_cpu_uv_high(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoCpuUVHigh);
    }
    static inline u32 cust_get_mariko_cpu_low_vmin(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoCpuLowVmin);
    }
    static inline u32 cust_get_mariko_cpu_high_vmin(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoCpuHighVmin);
    }
    static inline u32 cust_get_mariko_cpu_max_volt(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoCpuMaxVolt);
    }
    static inline u32 cust_get_erista_cpu_boost(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaCpuBoostClock);
    }
    static inline u32 cust_get_mariko_cpu_boost(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoCpuBoostClock);
    }
    static inline u32 cust_get_table_conf(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, tableConf);
    }

    static inline u32 cust_get_erista_gpu_uv(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaGpuUV);
    }
    static inline u32 cust_get_erista_gpu_vmin(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, eristaGpuVmin);
    }
    static inline u32 cust_get_mariko_gpu_uv(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoGpuUV);
    }
    static inline u32 cust_get_mariko_gpu_vmin(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoGpuVmin);
    }
    static inline u32 cust_get_mariko_gpu_vmax(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoGpuVmax);
    }
    static inline u32 cust_get_common_gpu_offset(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, commonGpuVoltOffset);
    }
    static inline u32 cust_get_marikoCpuMaxClock(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoCpuMaxClock);
    }
    static inline u32 cust_get_marikoSocVmax(const CustomizeTable *t) {
        return CUST_GET_FIELD(t, marikoSocVmax);
    }

    static inline u32 cust_get_erista_gpu_volt(const CustomizeTable *t, int idx) {
        if (!t || idx < 0 || idx >= 27)
            return 0;
        return t->eristaGpuVoltArray[idx];
    }

    static inline u32 cust_get_mariko_gpu_volt(const CustomizeTable *t, int idx) {
        if (!t || idx < 0 || idx >= 25)
            return 0;
        return t->marikoGpuVoltArray[idx];
    }

    static inline u32 cust_get_mariko_soc_volt(const CustomizeTable *t, int idx) {
        if (!t || idx < 0 || idx >= 26)
            return 0;
        return t->marikoSocVoltArray[idx];
    }

#define DECL_ERISTA_GPU_VOLT_HELPER(freq, idx)                                 \
    static inline bool cust_set_erista_gpu_volt_##freq(const char *p, u32 v) { \
        return cust_set_erista_gpu_volt(p, idx, v);                            \
    }

#define DECL_MARIKO_GPU_VOLT_HELPER(freq, idx)                                 \
    static inline bool cust_set_mariko_gpu_volt_##freq(const char *p, u32 v) { \
        return cust_set_mariko_gpu_volt(p, idx, v);                            \
    }

    #define DECL_MARIKO_SOC_VOLT_HELPER(freq, idx)                             \
        static inline bool cust_set_mariko_soc_volt_##freq(const char *p, u32 v) { \
            return cust_set_mariko_soc_volt(p, idx, v);                            \
        }

    DECL_ERISTA_GPU_VOLT_HELPER(76800,  0)
    DECL_ERISTA_GPU_VOLT_HELPER(115200, 1)
    DECL_ERISTA_GPU_VOLT_HELPER(153600, 2)
    DECL_ERISTA_GPU_VOLT_HELPER(192000, 3)
    DECL_ERISTA_GPU_VOLT_HELPER(230400, 4)
    DECL_ERISTA_GPU_VOLT_HELPER(268800, 5)
    DECL_ERISTA_GPU_VOLT_HELPER(307200, 6)
    DECL_ERISTA_GPU_VOLT_HELPER(345600, 7)
    DECL_ERISTA_GPU_VOLT_HELPER(384000, 8)
    DECL_ERISTA_GPU_VOLT_HELPER(422400, 9)
    DECL_ERISTA_GPU_VOLT_HELPER(460800, 10)
    DECL_ERISTA_GPU_VOLT_HELPER(499200, 11)
    DECL_ERISTA_GPU_VOLT_HELPER(537600, 12)
    DECL_ERISTA_GPU_VOLT_HELPER(576000, 13)
    DECL_ERISTA_GPU_VOLT_HELPER(614400, 14)
    DECL_ERISTA_GPU_VOLT_HELPER(652800, 15)
    DECL_ERISTA_GPU_VOLT_HELPER(691200, 16)
    DECL_ERISTA_GPU_VOLT_HELPER(729600, 17)
    DECL_ERISTA_GPU_VOLT_HELPER(768000, 18)
    DECL_ERISTA_GPU_VOLT_HELPER(806400, 19)
    DECL_ERISTA_GPU_VOLT_HELPER(844800, 20)
    DECL_ERISTA_GPU_VOLT_HELPER(883200, 21)
    DECL_ERISTA_GPU_VOLT_HELPER(921600, 22)
    DECL_ERISTA_GPU_VOLT_HELPER(960000, 23)
    DECL_ERISTA_GPU_VOLT_HELPER(998400, 24)
    DECL_ERISTA_GPU_VOLT_HELPER(1036800, 25)
    DECL_ERISTA_GPU_VOLT_HELPER(1075200, 26)

    DECL_MARIKO_GPU_VOLT_HELPER(76800, 0)
    DECL_MARIKO_GPU_VOLT_HELPER(153600, 1)
    DECL_MARIKO_GPU_VOLT_HELPER(230400, 2)
    DECL_MARIKO_GPU_VOLT_HELPER(307200, 3)
    DECL_MARIKO_GPU_VOLT_HELPER(384000, 4)
    DECL_MARIKO_GPU_VOLT_HELPER(460800, 5)
    DECL_MARIKO_GPU_VOLT_HELPER(537600, 6)
    DECL_MARIKO_GPU_VOLT_HELPER(614400, 7)
    DECL_MARIKO_GPU_VOLT_HELPER(691200, 8)
    DECL_MARIKO_GPU_VOLT_HELPER(768000, 9)
    DECL_MARIKO_GPU_VOLT_HELPER(844800, 10)
    DECL_MARIKO_GPU_VOLT_HELPER(921600, 11)
    DECL_MARIKO_GPU_VOLT_HELPER(998400, 12)
    DECL_MARIKO_GPU_VOLT_HELPER(1075200, 13)
    DECL_MARIKO_GPU_VOLT_HELPER(1152000, 14)
    DECL_MARIKO_GPU_VOLT_HELPER(1228800, 15)
    DECL_MARIKO_GPU_VOLT_HELPER(1267200, 16)
    DECL_MARIKO_GPU_VOLT_HELPER(1305600, 17)
    DECL_MARIKO_GPU_VOLT_HELPER(1344000, 18)
    DECL_MARIKO_GPU_VOLT_HELPER(1382400, 19)
    DECL_MARIKO_GPU_VOLT_HELPER(1420800, 20)
    DECL_MARIKO_GPU_VOLT_HELPER(1459200, 21)
    DECL_MARIKO_GPU_VOLT_HELPER(1497600, 22)
    DECL_MARIKO_GPU_VOLT_HELPER(1536000, 23)
    DECL_MARIKO_GPU_VOLT_HELPER(1574400, 24)

    DECL_MARIKO_SOC_VOLT_HELPER(1866000,  0)
    DECL_MARIKO_SOC_VOLT_HELPER(2000000,  1)
    DECL_MARIKO_SOC_VOLT_HELPER(2133000,  2)
    DECL_MARIKO_SOC_VOLT_HELPER(2200000,  3)
    DECL_MARIKO_SOC_VOLT_HELPER(2266000,  4)
    DECL_MARIKO_SOC_VOLT_HELPER(2333000,  5)
    DECL_MARIKO_SOC_VOLT_HELPER(2400000,  6)
    DECL_MARIKO_SOC_VOLT_HELPER(2433000,  7)
    DECL_MARIKO_SOC_VOLT_HELPER(2466000,  8)
    DECL_MARIKO_SOC_VOLT_HELPER(2533000,  9)
    DECL_MARIKO_SOC_VOLT_HELPER(2566000, 10)
    DECL_MARIKO_SOC_VOLT_HELPER(2600000, 11)
    DECL_MARIKO_SOC_VOLT_HELPER(2666000, 12)
    DECL_MARIKO_SOC_VOLT_HELPER(2700000, 13)
    DECL_MARIKO_SOC_VOLT_HELPER(2733000, 14)
    DECL_MARIKO_SOC_VOLT_HELPER(2766000, 15)
    DECL_MARIKO_SOC_VOLT_HELPER(2800000, 16)
    DECL_MARIKO_SOC_VOLT_HELPER(2833000, 17)
    DECL_MARIKO_SOC_VOLT_HELPER(2900000, 18)
    DECL_MARIKO_SOC_VOLT_HELPER(2933000, 19)
    DECL_MARIKO_SOC_VOLT_HELPER(3000000, 20)
    DECL_MARIKO_SOC_VOLT_HELPER(3033000, 21)
    DECL_MARIKO_SOC_VOLT_HELPER(3100000, 22)
    DECL_MARIKO_SOC_VOLT_HELPER(3133000, 23)
    DECL_MARIKO_SOC_VOLT_HELPER(3166000, 24)
    DECL_MARIKO_SOC_VOLT_HELPER(3200000, 25)
    DECL_MARIKO_SOC_VOLT_HELPER(3266000, 26)
    DECL_MARIKO_SOC_VOLT_HELPER(3333000, 27)

#define DECL_ERISTA_GPU_VOLT_GET(freq, idx)                                  \
    static inline u32 cust_get_erista_gpu_volt_##freq##_val(const char *p) { \
        CustomizeTable t;                                                    \
        if (!cust_read_table(p, &t))                                         \
            return 0;                                                        \
        return cust_get_erista_gpu_volt(&t, idx);                            \
    }
#define DECL_MARIKO_GPU_VOLT_GET(freq, idx)                                  \
    static inline u32 cust_get_mariko_gpu_volt_##freq##_val(const char *p) { \
        CustomizeTable t;                                                    \
        if (!cust_read_table(p, &t))                                         \
            return 0;                                                        \
        return cust_get_mariko_gpu_volt(&t, idx);                            \
    }

    #define DECL_MARIKO_SOC_VOLT_GET(freq, idx)                                  \
        static inline u32 cust_get_mariko_soc_volt_##freq##_val(const char *p) { \
            CustomizeTable t;                                                    \
            if (!cust_read_table(p, &t))                                         \
                return 0;                                                        \
            return cust_get_mariko_soc_volt(&t, idx);                            \
        }

    DECL_ERISTA_GPU_VOLT_GET(76800, 0)
    DECL_ERISTA_GPU_VOLT_GET(115200, 1)
    DECL_ERISTA_GPU_VOLT_GET(153600, 2)
    DECL_ERISTA_GPU_VOLT_GET(192000, 3)
    DECL_ERISTA_GPU_VOLT_GET(230400, 4)
    DECL_ERISTA_GPU_VOLT_GET(268800, 5)
    DECL_ERISTA_GPU_VOLT_GET(307200, 6)
    DECL_ERISTA_GPU_VOLT_GET(345600, 7)
    DECL_ERISTA_GPU_VOLT_GET(384000, 8)
    DECL_ERISTA_GPU_VOLT_GET(422400, 9)
    DECL_ERISTA_GPU_VOLT_GET(460800, 10)
    DECL_ERISTA_GPU_VOLT_GET(499200, 11)
    DECL_ERISTA_GPU_VOLT_GET(537600, 12)
    DECL_ERISTA_GPU_VOLT_GET(576000, 13)
    DECL_ERISTA_GPU_VOLT_GET(614400, 14)
    DECL_ERISTA_GPU_VOLT_GET(652800, 15)
    DECL_ERISTA_GPU_VOLT_GET(691200, 16)
    DECL_ERISTA_GPU_VOLT_GET(729600, 17)
    DECL_ERISTA_GPU_VOLT_GET(768000, 18)
    DECL_ERISTA_GPU_VOLT_GET(806400, 19)
    DECL_ERISTA_GPU_VOLT_GET(844800, 20)
    DECL_ERISTA_GPU_VOLT_GET(883200, 21)
    DECL_ERISTA_GPU_VOLT_GET(921600, 22)
    DECL_ERISTA_GPU_VOLT_GET(960000, 23)
    DECL_ERISTA_GPU_VOLT_GET(998400, 24)
    DECL_ERISTA_GPU_VOLT_GET(1036800, 25)
    DECL_ERISTA_GPU_VOLT_GET(1075200, 26)

    DECL_MARIKO_GPU_VOLT_GET(76800, 0)
    DECL_MARIKO_GPU_VOLT_GET(153600, 1)
    DECL_MARIKO_GPU_VOLT_GET(230400, 2)
    DECL_MARIKO_GPU_VOLT_GET(307200, 3)
    DECL_MARIKO_GPU_VOLT_GET(384000, 4)
    DECL_MARIKO_GPU_VOLT_GET(460800, 5)
    DECL_MARIKO_GPU_VOLT_GET(537600, 6)
    DECL_MARIKO_GPU_VOLT_GET(614400, 7)
    DECL_MARIKO_GPU_VOLT_GET(691200, 8)
    DECL_MARIKO_GPU_VOLT_GET(768000, 9)
    DECL_MARIKO_GPU_VOLT_GET(844800, 10)
    DECL_MARIKO_GPU_VOLT_GET(921600, 11)
    DECL_MARIKO_GPU_VOLT_GET(998400, 12)
    DECL_MARIKO_GPU_VOLT_GET(1075200, 13)
    DECL_MARIKO_GPU_VOLT_GET(1152000, 14)
    DECL_MARIKO_GPU_VOLT_GET(1228800, 15)
    DECL_MARIKO_GPU_VOLT_GET(1267200, 16)
    DECL_MARIKO_GPU_VOLT_GET(1305600, 17)
    DECL_MARIKO_GPU_VOLT_GET(1344000, 18)
    DECL_MARIKO_GPU_VOLT_GET(1382400, 19)
    DECL_MARIKO_GPU_VOLT_GET(1420800, 20)
    DECL_MARIKO_GPU_VOLT_GET(1459200, 21)
    DECL_MARIKO_GPU_VOLT_GET(1497600, 22)
    DECL_MARIKO_GPU_VOLT_GET(1536000, 23)
    DECL_MARIKO_GPU_VOLT_GET(1574400, 24)

    DECL_MARIKO_SOC_VOLT_GET(1866000,  0)
    DECL_MARIKO_SOC_VOLT_GET(2000000,  1)
    DECL_MARIKO_SOC_VOLT_GET(2133000,  2)
    DECL_MARIKO_SOC_VOLT_GET(2200000,  3)
    DECL_MARIKO_SOC_VOLT_GET(2266000,  4)
    DECL_MARIKO_SOC_VOLT_GET(2333000,  5)
    DECL_MARIKO_SOC_VOLT_GET(2400000,  6)
    DECL_MARIKO_SOC_VOLT_GET(2433000,  7)
    DECL_MARIKO_SOC_VOLT_GET(2466000,  8)
    DECL_MARIKO_SOC_VOLT_GET(2533000,  9)
    DECL_MARIKO_SOC_VOLT_GET(2566000, 10)
    DECL_MARIKO_SOC_VOLT_GET(2600000, 11)
    DECL_MARIKO_SOC_VOLT_GET(2666000, 12)
    DECL_MARIKO_SOC_VOLT_GET(2700000, 13)
    DECL_MARIKO_SOC_VOLT_GET(2733000, 14)
    DECL_MARIKO_SOC_VOLT_GET(2766000, 15)
    DECL_MARIKO_SOC_VOLT_GET(2800000, 16)
    DECL_MARIKO_SOC_VOLT_GET(2833000, 17)
    DECL_MARIKO_SOC_VOLT_GET(2900000, 18)
    DECL_MARIKO_SOC_VOLT_GET(2933000, 19)
    DECL_MARIKO_SOC_VOLT_GET(3000000, 20)
    DECL_MARIKO_SOC_VOLT_GET(3033000, 21)
    DECL_MARIKO_SOC_VOLT_GET(3100000, 22)
    DECL_MARIKO_SOC_VOLT_GET(3133000, 23)
    DECL_MARIKO_SOC_VOLT_GET(3166000, 24)
    DECL_MARIKO_SOC_VOLT_GET(3200000, 25)
    DECL_MARIKO_SOC_VOLT_GET(3266000, 26)
    DECL_MARIKO_SOC_VOLT_GET(3333000, 27)

    void MigrateKipData(u32 custRev, u32 version);
    void SetKipData();
    void GetKipData();
}  // namespace kip