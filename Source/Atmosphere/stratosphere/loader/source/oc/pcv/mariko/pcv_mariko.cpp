/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) B3711
 *
 * Copyright (c) Souldbminer, Lightos and Horizon OC Contributors
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

#include <vector>
#include "../pcv.hpp"
#include "../../mtc_timing_value.hpp"
#include "pcv_mariko.hpp"
#include "pcv_mariko_cpu.hpp"
#include "pcv_mariko_gpu.hpp"
#include "pcv_mariko_mtc.hpp"
#include "pcv_mariko_soc.hpp"
#include "pcv_mariko_log.hpp"
#include "calculate_timings_mariko.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    u32 *nsoStart;
    size_t g_nso_size = 0;

    DEFINE_HOOK_PAYLOAD_PTR(HookPayloadData, m_HookPayloadData);

    Result InstallHooks() {
        R_TRY(Hooks().CheckEnabled());

        R_TRY(Hooks().CopyPayload());

        auto *data = Hooks().BindData(m_HookPayloadData);
        R_UNLESS(data != nullptr, ldr::ResultHookDataOutOfMemory());

        R_TRY(SharedClkBusInstallHooks(data));

#if HOC_UART_LOG
        R_TRY(ForceVebosityInstallHooks(data));
#endif

        R_SUCCEED();
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        nsoStart   = reinterpret_cast<u32 *>(mapped_nso);
        g_nso_size = nso_size;

        MtcGenerateFreqTables();

        u32 CpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(CpuCvbTableDefault)->freq);
        u32 GpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(GpuCvbTableDefault)->freq);

        PatcherEntry<u32> patches[] = {
            { "CPU Freq Vdd",      &CpuFreqVdd,            1, nullptr,  CpuClkOSLimit              },
            { "CPU Freq Table",     CpuFreqCvbTable<true>, 1, nullptr,  CpuCvbDefaultMaxFreq       },
            { "CPU Volt DVFS",     &CpuVoltDVFS,           1, nullptr,  CpuVminOfficial            },
            { "CPU Volt Thermals", &CpuVoltThermals,       1, nullptr,  CpuVminOfficial            },
            { "CPU Volt Dfll",     &CpuVoltDfll,           1, nullptr,  CpuTune0Low                },
            { "GPU Volt DVFS",     &GpuVoltDVFS,           1, nullptr,  GpuVminOfficial            },
            { "GPU Volt Thermals", &GpuVoltThermals,       1, nullptr,  GpuVminOfficial            },
            { "GPU Freq Table",     GpuFreqCvbTable<true>, 1, nullptr,  GpuCvbDefaultMaxFreq       },
            { "GPU Freq Asm",      &GpuFreqMaxAsm,         2,          &GpuMaxClockPatternFn       },
            { "GPU PLL Max",       &GpuFreqPllMax,         1, nullptr,  GpuClkPllMax               },
            { "GPU PLL Limit",     &GpuFreqPllLimit,       4, nullptr,  GpuClkPllLimit             },
            { "MEM Freq Mtc",      &MemFreqMtcTable,       1, nullptr,  EmcClkOSLimit              },
            { "MEM Freq Dvb",      &MemFreqDvbTable,       1, nullptr,  EmcClkOSLimit              },
            { "MEM Freq Max",      &MemFreqMax,            0, nullptr,  EmcClkOSLimit              },
            { "MEM Freq PLLM",     &MemFreqPllmLimit,      2, nullptr,  EmcClkPllmLimit            },
            { "MEM Vddq",          &EmcVddqVolt,           2, nullptr,  EmcVddqDefault             },
            { "MEM Vdd2",          &MemVoltHandler,        2, nullptr,  MemVdd2Default             },
            { "MEM Table Asm",     &MemMtcTableAsm,        1,          &MemMtcGetGetTablePatternFn },
            { "EMC DVFS Count",    &EmcDvfsCountLimit,     1,          &EmcDvfsCountPatternFn      },
            { "EMC SoC LUT",       &EmcSocLutReloc,        1,          &EmcSocLutPatternFn         },
            { "EMC Rate List",     &EmcRateListLimit,      0,          &EmcRateListPatternFn       },
            { "EMC Rate Sess",     &EmcRateSessLimit,      1,          &EmcRateSessPatternFn       },
            { "Bus Freq Reloc",    &BusFreqReloc,          1,          &BusFreqRelocPatternFn      },
            { "SOC Volt Asm",      &SocVoltAsm,            1,          &SocVoltPatternFn           },
            { "SOC Volt Limit",    &SocVoltLimit,          1, nullptr,  SocVoltLimitOfficial       },
            /* Debugging patches */
            #if HOC_UART_LOG
            { "NvLog Redirect",    &NvLogUartRedirect,     1,          &NvLogVsnprintfPatternFn,   0, 0, true },
            { "Force Verbosity",   &ForceVerbosity,        3,          &ForceVerbosityPatternFn,   0, 0, true },
            #endif
        };

        for (uintptr_t ptr = mapped_nso; ptr <= mapped_nso + nso_size - sizeof(MarikoMtcTable); ptr += sizeof(u32)) {
            u32 *ptr32 = reinterpret_cast<u32 *>(ptr);
            for (auto &entry : patches) {
                if (R_SUCCEEDED(entry.SearchAndApply(ptr32))) {
                    break;
                }
            }
        }

        for (auto &entry : patches) {
            LOGGING("%s Count: %zu", entry.description, entry.patched_count);
            if (R_FAILED(entry.CheckResult())) {
                panic::SmcError(panic::Patch);

                CRASH(entry.description);
            }
        }

        if (R_FAILED(InstallHooks())) {
            panic::SmcError(panic::Patch);
        }
    }

}
