/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) B3711
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

#include "../pcv.hpp"
#include "../../mtc_timing_value.hpp"
#include "pcv_erista_cpu.hpp"
#include "pcv_erista_gpu.hpp"
#include "pcv_erista_mtc.hpp"
#include "calculate_timings_erista.hpp"

namespace ams::ldr::hoc::pcv::erista {

    DEFINE_HOOK_PAYLOAD_PTR(HookPayloadData, e_HookPayloadData);

    u32 *nsoStart;

    Result InstallHooks() {
        R_TRY(Hooks().CheckEnabled());

        R_TRY(Hooks().CopyPayload());

        auto *data = Hooks().BindData(e_HookPayloadData);
        R_UNLESS(data != nullptr, ldr::ResultHookDataOutOfMemory());

        R_TRY(MtcInstallHooks(data));

        R_SUCCEED();
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        nsoStart = reinterpret_cast<u32 *>(mapped_nso);
        MtcGenerateFreqTables();

        u32 CpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(CpuCvbTableDefault)->freq);
        u32 GpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(GpuCvbTableDefault)->freq);

        PatcherEntry<u32> patches[] = {
            {"CPU Freq Table",     CpuFreqCvbTable<false>, 1, nullptr,  CpuCvbDefaultMaxFreq },
            {"CPU Volt DVFS",     &CpuVoltDvfs,            1, nullptr,  CpuVminOfficial      },
            {"CPU Volt Thermals", &CpuVoltThermals,        1, nullptr,  CpuVminOfficial      },
            {"CPU Volt Dfll",     &CpuVoltDfll,            1, nullptr,  CpuTune0Low          },
            {"GPU Volt DVFS",     &GpuVoltDVFS,            1, nullptr,  GpuVminOfficial      },
            {"GPU Volt Thermals", &GpuVoltThermals,        1, nullptr,  GpuVminOfficial      },
            {"GPU Freq Table",     GpuFreqCvbTable<false>, 1, nullptr,  GpuCvbDefaultMaxFreq },
            {"GPU Freq Asm",      &GpuFreqMaxAsm,          2,          &GpuMaxClockPatternFn },
            {"GPU PLL Max",       &GpuFreqPllMax,          1, nullptr,  GpuClkPllMax         },
            // {"GPU PLL Limit",  &GpuFreqPllLimit,        4, nullptr,  GpuClkPllLimit       },
            {"MEM Table Asm",     &MemMtcTableAsm,         4,           &MemMtcGetGetTablePatternFn },
            {"MEM Freq Mtc",      &MemFreqMtcTable,        1, nullptr,  EmcClkOSLimit        },
            {"MEM Freq Max",      &MemFreqMax,             0, nullptr,  EmcClkOSLimit        },
            {"MEM Freq PLLM",     &MemFreqPllmLimit,       2, nullptr,  EmcClkPllmLimit      },
            {"MEM Volt",          &MemVoltHandler,         2, nullptr,  MemVoltHOS           },
        };

        for (uintptr_t ptr = mapped_nso; ptr <= mapped_nso + nso_size - sizeof(EristaMtcTable); ptr += sizeof(u32)) {
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
