/*
 * Copyright (C) Switch-OC-Suite
 *
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
 */

#pragma once

#include <stratosphere.hpp>
#include <vapours/results/results_common.hpp>

#define HOC_UART_LOG 1
#define HOC_PCV_NVLOG_PATCH 1
#define HOC_PCV_FORCE_VERBOSITY 1

#include "customize.hpp"
#include "oc_log.hpp"

#if (!HOC_UART_LOG) && (defined(AMS_BUILD_FOR_AUDITING) || defined(AMS_BUILD_FOR_DEBUGGING))
    #define HOC_IRAM_LOG 1
#endif

#if defined(AMS_BUILD_FOR_AUDITING) || defined(AMS_BUILD_FOR_DEBUGGING)

    #if defined(HOC_IRAM_LOG)
        #define LOGGING(...) Log(__VA_ARGS__)
    #elif HOC_UART_LOG
        #define LOGGING(...) AMS_LOG(__VA_ARGS__)
    #endif

#else
    #define LOGGING(...) ((void)0)
#endif

#define CRASH(msg, ...) { ams::diag::AbortImpl(msg, __PRETTY_FUNCTION__, "", 0); __builtin_unreachable(); }

#define PATCH_OFFSET(offset, value) \
    static_assert(sizeof(__typeof__(offset)) <= sizeof(u64)); \
    *(offset) = value;

namespace ams::ldr {
    R_DEFINE_ERROR_RESULT(OutOfRange,                1000);
    R_DEFINE_ERROR_RESULT(InvalidMemPllmEntry,       1001);
    R_DEFINE_ERROR_RESULT(InvalidMtcMagic,           1002);
    R_DEFINE_ERROR_RESULT(InvalidMtcTable,           1003);
    R_DEFINE_ERROR_RESULT(InvalidDvbTable,           1004);
    R_DEFINE_ERROR_RESULT(InvalidCpuFreqVddEntry,    1005);
    R_DEFINE_ERROR_RESULT(InvalidCpuVoltDfllEntry,   1006);
    R_DEFINE_ERROR_RESULT(InvalidCpuDvfs,            1007);
    R_DEFINE_ERROR_RESULT(InvalidCpuMinVolt,         1008);
    R_DEFINE_ERROR_RESULT(InvalidGpuDvfs,            1009);
    R_DEFINE_ERROR_RESULT(InvalidGpuFreqMaxPattern,  1010);
    R_DEFINE_ERROR_RESULT(InvalidGpuPllEntry,        1011);
    R_DEFINE_ERROR_RESULT(InvalidRegulatorEntry,     1012);
    R_DEFINE_ERROR_RESULT(UninitializedPatcher,      1013);
    R_DEFINE_ERROR_RESULT(UnsuccessfulPatcher,       1014);
    R_DEFINE_ERROR_RESULT(SafetyCheckFailure,        1015);
    R_DEFINE_ERROR_RESULT(InvalidMtcTablePattern,    1016);
    R_DEFINE_ERROR_RESULT(InvalidSocVoltPattern,     1017);
    R_DEFINE_ERROR_RESULT(InvalidSocVoltLimit,       1018);
    R_DEFINE_ERROR_RESULT(InvalidEmcDvfsCount,       1019);
    R_DEFINE_ERROR_RESULT(InvalidEmcSocLut,          1020);
    R_DEFINE_ERROR_RESULT(InvalidEmcRateList,        1021);
    R_DEFINE_ERROR_RESULT(InvalidNvLogRedirect,      1022);
    R_DEFINE_ERROR_RESULT(InvalidBusFreqReloc,       1023);
    R_DEFINE_ERROR_RESULT(HookArenaOutOfMemory,      1024);
    R_DEFINE_ERROR_RESULT(HookPayloadTooLarge,       1025);
    R_DEFINE_ERROR_RESULT(HookRelocationUnsupported, 1026);
    R_DEFINE_ERROR_RESULT(HookSiteInvalid,           1027);
    R_DEFINE_ERROR_RESULT(HookPayloadEscapes,        1028);
    R_DEFINE_ERROR_RESULT(HookDataOutOfMemory,       1029);
    R_DEFINE_ERROR_RESULT(HookUnavailable,           1030);
}

namespace ams::ldr::hoc {
    template<typename Pointer>
    struct PatcherEntry {
        using patternFn = bool(*)(Pointer *ptr);
        using patcherFn = Result(*)(Pointer *ptr);

        const char* description;
        patcherFn   patcher_fn = nullptr;
        size_t      maximum_patched_count = 0;
        patternFn   pattern_search_fn = nullptr;
        Pointer     value_search;
        size_t      patched_count = 0;
        bool        optional = false;

        Result Apply(Pointer *ptr) {
            Result res = patcher_fn(ptr);
            if (R_SUCCEEDED(res)) {
                patched_count++;
            }

            return res;
        }

        Result SearchAndApply(Pointer *ptr) {
            bool searchOk = false;
            if (pattern_search_fn) {
                if (pattern_search_fn(ptr)) {
                    searchOk = true;
                }
            } else {
                if (value_search == *(ptr)) {
                    searchOk = true;
                }
            }

            if (searchOk) {
                return Apply(ptr);
            }

            R_THROW(ldr::ResultUnsuccessfulPatcher());
        }

        Result CheckResult() {
            R_UNLESS(optional || patched_count > 0, ldr::ResultUnsuccessfulPatcher());

            if (maximum_patched_count) {
                R_UNLESS(patched_count <= maximum_patched_count, ldr::ResultUnsuccessfulPatcher());
            }

            R_SUCCEED();
        }
    };

    namespace panic {
        /* Requires modifying g_ams_handlers in secmon_smc_handler.cpp */
        constexpr inline void SmcError(u32 rgb) {
            SecmonArgs args = {};
            constexpr u32 SmcShowErrorID = 0xF0000005;
            args.X[0] = SmcShowErrorID;
            args.X[1] = rgb;
            svcCallSecureMonitor(&args);
        }

        constexpr inline u32 PackCode(u32 r, u32 g, u32 b) {
            return ((r & 0xF) << 8) | ((g & 0xF) << 4) | ((b & 0xF) << 0);
        }

        constexpr u32 Gpu   = PackCode(0xF, 0x7, 0x0);
        constexpr u32 Cpu   = PackCode(0xF, 0x0, 0x0);
        constexpr u32 Emc   = PackCode(0x0, 0xF, 0xF);
        constexpr u32 Patch = PackCode(0x8, 0x0, 0xF);
    }

}
