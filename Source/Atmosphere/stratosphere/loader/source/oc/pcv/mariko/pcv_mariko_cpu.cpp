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

#include "../pcv.hpp"
#include "pcv_mariko_cpu.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    u32 CapCpuClock() {
        u32 cpuCap = allowedCpuMaxFrequencies[0];

        for (u32 freq : allowedCpuMaxFrequencies) {
            if (C.marikoCpuMaxClock >= freq) {
                cpuCap = freq;
            } else {
                break;
            }
        }
        return cpuCap;
    }

    Result CpuFreqVdd(u32 *ptr) {
        dvfs_rail *entry = reinterpret_cast<dvfs_rail *>(reinterpret_cast<u8 *>(ptr) - offsetof(dvfs_rail, freq));

        R_UNLESS(entry->id      == 1,        ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->min_mv  == 250'000,  ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->step_mv == 5000,     ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->max_mv  == 1525'000, ldr::ResultInvalidCpuFreqVddEntry());

        if (C.marikoCpuUVHigh) {
            PATCH_OFFSET(ptr, CapCpuClock());
        } else {
            PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.marikoCpuDvfsTable)->freq);
        }

        R_SUCCEED();
    }

    Result CpuVoltDVFS(u32 *ptr) {
        CvbMeta *cpuCvbMeta = reinterpret_cast<CvbMeta *>(reinterpret_cast<u8 *>(ptr) - offsetof(CvbMeta, vmin));

        R_UNLESS(cpuCvbMeta->highVmin     == CpuHighVminOfficial, ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkStepMaybe == 38,                  ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->vmax         == CpuVoltOfficial,     ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkScale2    == 1000,                ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->speedoScale  == 100,                 ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->voltageScale == 1000,                ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkZero5     == 0,                   ldr::ResultInvalidCpuMinVolt());

        if (C.marikoCpuLowVmin) {
            PATCH_OFFSET(&(cpuCvbMeta->vmin), C.marikoCpuLowVmin);
        }

        if (C.marikoCpuHighVmin) {
            PATCH_OFFSET(&(cpuCvbMeta->highVmin), C.marikoCpuHighVmin);
        }

        if (C.marikoCpuMaxVolt) {
            PATCH_OFFSET(&(cpuCvbMeta->vmax), C.marikoCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr, cpuVoltThermalData, sizeof(cpuVoltThermalData))) {
            R_THROW(ldr::ResultInvalidCpuMinVolt());
        }

        if (C.marikoCpuLowVmin) {
            PATCH_OFFSET(ptr,     C.marikoCpuLowVmin);
            PATCH_OFFSET(ptr + 3, C.marikoCpuLowVmin);
        }

        if (C.marikoCpuMaxVolt) {
            PATCH_OFFSET(ptr - 2, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr - 5, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr + 1, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr + 4, C.marikoCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltDfll(u32 *ptr) {
        CvbCpuDfllData *entry = reinterpret_cast<CvbCpuDfllData *>(ptr);

        R_UNLESS(entry->tune0_low  == 0xFFCF,    ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune0_high == 0x0,       ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_low  == 0x12207FF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_high == 0x3FFF7FF, ldr::ResultInvalidCpuVoltDfllEntry());

        switch (C.marikoCpuUVLow) {
            case 1:
                PATCH_OFFSET(&(entry->tune0_low),  0xffa0);
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x0);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27207ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_low),  0xffdf);
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27307ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_low),  0xffff);
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27707ff);
                break;
            case 6:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 7:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 8:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 9:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27c07ff);
                break;
            case 10:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27d07ff);
                break;
            case 11:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27e07ff);
                break;
            case 12:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27f07ff);
                break;
            default:
                break;
        }

        switch (C.marikoCpuUVHigh) {
            case 1:
                PATCH_OFFSET(&(entry->tune1_high), 0x0);
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27207ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27307ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27707ff);
                break;
            case 6:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 7:
            case 8:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 9:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27c07ff);
                break;
            case 10:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27d07ff);
                break;
            case 11:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27e07ff);
                break;
            case 12:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27f07ff);
                break;
            default:
                break;
        }

        R_SUCCEED();
    }

    /* Little helper for ORR:LSL pairs which are found in here */
    inline bool IsOrrLsl(u32 w) {
        if (!_asm::IsOp(w, _asm::op::OrrShifted32, _asm::field::Rd, _asm::field::Rn, _asm::field::Rm, _asm::field::Shift, _asm::field::Imm6))
            return false;
        return _asm::Get(w, _asm::field::Shift) == 0; /* 0=LSL, 1=LSR, 2=ASR, 3=ROR */
    }

    /* Probably overcomplicated but I wanted to have a rock-solid way to do this regardless of FW version */
    Result CpuLutMaxAsm(u32* ptr) {
        /* Check for the dual ORR-LSL pairs*/
        R_UNLESS(IsOrrLsl(ptr[2]), ldr::ResultInvalidCpuLutMaxAsmPattern());
        R_UNLESS(IsOrrLsl(ptr[3]), ldr::ResultInvalidCpuLutMaxAsmPattern());
        /* We can safely assume that we are in the right place now */

        /* Safety check */
        R_UNLESS(ptr - 10 >= nsoStart, ldr::ResultInvalidCpuLutMaxAsmPattern());

        /* Search for the STRB */
        u32* strb = _asm::ScanAssembly(ptr - 10, 10, _asm::Encode(_asm::op::StrbImm,
            {_asm::field::Rt, 0}, {_asm::field::Rn, 0}, {_asm::field::Off1, 0x25D}), _asm::field::Rt,
            _asm::field::Rn);
        R_UNLESS(strb != nullptr, ldr::ResultInvalidCpuLutMaxAsmPattern());

        /* Probably a cleaner way to do this?*/
        u32 *prod = strb + 2;
        const u32 rd = _asm::Get(*prod, _asm::field::Rd);
        if (_asm::IsOp(*prod, _asm::op::Ubfm32, _asm::field::Rd, _asm::field::Rn, _asm::field::Immr, _asm::field::Imm6)) {
            /* replacement must carry LSL#16 to make up for ubfiz's remoal */
            const u32 nw = _asm::Encode(_asm::op::MovzW, {_asm::field::Rd, rd}, {_asm::field::Imm16, 0x3F}, {_asm::field::Hw, 1});
            PATCH_OFFSET(prod, nw); /* movz wD,#0x3F,LSL#16 */
        } else {
            /* Imm12 is 7 for 0xff*/
            R_UNLESS(_asm::IsOp(*prod, _asm::op::AndImm32, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12) && _asm::Get(*prod, _asm::field::Imm12) == 7, ldr::ResultInvalidCpuLutMaxAsmPattern());
            /* No need to do the lsl */
            const u32 nw = _asm::Encode(_asm::op::MovzW, {_asm::field::Rd, rd}, {_asm::field::Imm16, 0x3F});
            PATCH_OFFSET(prod, nw); /* movz wD,#0x3F */
        }

        /* Set SAFE to entry 32 */
        PATCH_OFFSET(ptr + 1, _asm::Encode(_asm::op::MovzW, {_asm::field::Rd, _asm::Get(ptr[1], _asm::field::Rd)}, {_asm::field::Imm16, 32}));

        R_SUCCEED();
    }

    namespace {
        struct {
            u32 *site = nullptr; /* LUT writer prologue. */
        } lutWriterCache;
    }

    HOOK_PAYLOAD_FN void CpuLutWriterExpandImpl(u64 param) {
        HookPayloadData *data = HOOK_PAYLOAD_PTR(HookPayloadData, m_HookPayloadData);
        reinterpret_cast<void (*)(u64)>(data->lut64.orig)(param);

        /* *(param+8) = CLDVFS mapping */
        const u64 base = *reinterpret_cast<u64 *>(param + 8);
        u32 *lut = reinterpret_cast<u32 *>(base + 0x200);

        /* Expand the LUT in place */
        const u32 top = lut[32];

        for (int i = 31; i >= 0; --i) {
            const u32 a = lut[i];
            const u32 b = lut[i + 1]; /* dst 2*(i+1) > src i+1 */
            lut[2 * i]     = a;
            lut[2 * i + 1] = (a + b) >> 1;
        }

        lut[63] = top;
        __asm__ volatile("dsb st" ::: "memory");
    }

    Result LutWriterFind(u32 *ptr) {
        R_UNLESS(LutWriterPatternFn(ptr), ldr::ResultInvalidCpuLutHook());

        u32 *prog = _asm::FindFnPrologue(ptr, 140, nsoStart);
        R_UNLESS(prog != nullptr, ldr::ResultInvalidCpuLutHook());

        /* Prolouge must hold MMIO base, so obtain it. */
        bool ok = false;
        for (u32 i = 0; i < 8; ++i) {
            if (_asm::IsOp(prog[i], _asm::op::LdrImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Off8)
                && _asm::Get(prog[i], _asm::field::Rn) == 0
                && _asm::Get(prog[i], _asm::field::Off8) == 0x8) {
                ok = true;
                break;
            }
        }

        R_UNLESS(ok, ldr::ResultInvalidCpuLutHook());
        lutWriterCache.site = prog;

        R_SUCCEED();
    }

    Result LutWriterInstallHooks(HookPayloadData *data) {
        R_UNLESS(lutWriterCache.site != nullptr, ldr::ResultInvalidCpuLutHook());

        uintptr_t orig = 0;
        R_TRY(INSTALL_IMPL_HOOK_ORIG(lutWriterCache.site, CpuLutWriterExpandImpl, &orig));
        data->lut64.orig = orig;

        R_SUCCEED();
    }

    Result CpuLutMaxAsm2(u32* ptr) {
        /* movk w?,#0xFFC0,LSL#16 */
        R_UNLESS(_asm::IsOp(ptr[1], _asm::op::MovkW, _asm::field::Rd, _asm::field::Imm16, _asm::field::Hw)
            && _asm::Get(ptr[1], _asm::field::Imm16) == 0xFFC0
            && _asm::Get(ptr[1], _asm::field::Hw) == 1, ldr::ResultInvalidCpuLutMaxAsmPattern());

        /* and x, x, #0xff */
        R_UNLESS(_asm::IsOp(ptr[2], _asm::op::AndImm32, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12) ||
            _asm::Get(ptr[2], _asm::field::Imm12) != 7, ldr::ResultInvalidCpuLutMaxAsmPattern());

        const u32 rd = _asm::Get(ptr[2], _asm::field::Rd);
        PATCH_OFFSET(ptr + 2, _asm::Encode(_asm::op::MovzW, {_asm::field::Rd, rd}, {_asm::field::Imm16, 0x3F})); /* movz wD,#0x3F */

        R_SUCCEED();
    }


    /* FW 20.4.0
      71000571dc 01 01 00 d0     adrp       param_2=>aCldvfssetdvcor,aSSRequestedDKh+0x1f    = "ClDvfsSetDvcoRateMin"
                                                                                             = " %s divider %#x diff %d\n"
      71000571e0 21 40 27 91     add        param_2=>aCldvfssetdvcor,param_2,#0x9d0          = "ClDvfsSetDvcoRateMin"
      71000571e4 c3 9f ff 97     bl         nn::pcv::NvLog                                   undefined NvLog(char * fmt, ...)
                             loc_71000571E8                                  XREF[1]:     71000571b4(j)
      71000571e8 69 aa 42 a9     ldp        x9,x10,[x19, #0x28]
      71000571ec 4a fd 41 d3     lsr        x10,x10,#0x1
      71000571f0 68 26 41 f9     ldr        x8,[x19, #0x248]                                 Anchor
      71000571f4 0b 0d 0a cb     sub        x11,x8,x10, LSL #0x3                             Target
      71000571f8 6b 4e 01 f9     str        x11,[x19, #0x298]                                Verify1
      71000571fc 2c 0d 40 f9     ldr        x12,[x9, #0x18]                                  Verify2
      7100057200 7f 01 0c eb     cmp        x11,x12
      7100057204 6b 81 8c 9a     csel       x11,x11,x12,hi
      7100057208 6b 4e 01 f9     str        x11,[x19, #0x298]
      710005720c 2b c9 40 f9     ldr        x11,[x9, #0x190]
      7100057210 6b 52 01 f9     str        x11,[x19, #0x2a0]
      7100057214 8b 00 00 b4     cbz        x11,loc_7100057224
      7100057218 f4 4f 41 a9     ldp        x20,x19,[sp, #local_10]
      710005721c fd 7b c2 a8     ldp        x29=>local_20,x30,[sp], #0x20
      7100057220 c0 03 5f d6     ret
                             loc_7100057224                                  XREF[1]:     7100057214(j)
      7100057224 08 15 0a 8b     add        x8,x8,x10, LSL #0x5                              Target
      7100057228 2a 55 81 b9     ldrsw      x10,[x9, #0x154]                                 Anchor
      710005722c 68 52 01 f9     str        x8,[x19, #0x2a0]                                 Verify1

    */

    Result CpuLutDvcoRateCfg(u32* ptr) {
        /* Anchor via scanning to prevent ordering issues */
        u32 *sub = _asm::ScanAssembly(ptr + 1, 4,
            _asm::Encode(_asm::op::SubShifted64, {_asm::field::Rd, 0}, {_asm::field::Rn, 0}, {_asm::field::Rm, 0},
                          {_asm::field::Shift, 0}, {_asm::field::Imm6, 3}),
            _asm::field::Rd, _asm::field::Rn, _asm::field::Rm);
        R_UNLESS(sub != nullptr, ldr::ResultInvalidDvcoRateConfig());

        /* Verify */
        R_UNLESS(_asm::IsOp(sub[1], _asm::op::StrImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Off8)
            && _asm::Get(sub[1], _asm::field::Off8) == 0x298, ldr::ResultInvalidDvcoRateConfig());

        /* 22.x uses pre-index ldr x?,[x?,#0x18]! here (0xF8418D6D class). */
        const bool isLdr18 = _asm::IsOp(sub[2], _asm::op::LdrImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Off8)
            && _asm::Get(sub[2], _asm::field::Off8) == 0x18;
        const bool isLdrPre18 = _asm::IsOp(sub[2], _asm::op::LdrPreImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Imm9)
            && _asm::Get(sub[2], _asm::field::Imm9) == 0x18;
        R_UNLESS(isLdr18 || isLdrPre18, ldr::ResultInvalidDvcoRateConfig());

        /* Patch the target */
        PATCH_OFFSET(sub, _asm::Encode(_asm::op::SubShifted64,
            {_asm::field::Rd, _asm::Get(*sub, _asm::field::Rd)},
            {_asm::field::Rn, _asm::Get(*sub, _asm::field::Rn)},
            {_asm::field::Rm, _asm::Get(*sub, _asm::field::Rm)},
            {_asm::field::Shift, 0}, {_asm::field::Imm6, 4}));

        /* Search for the second anchor */
        u32* ldrsw = _asm::ScanAssembly(sub, 20,
            _asm::Encode(_asm::op::LdrsWImm64, {_asm::field::Rt, 0}, {_asm::field::Rn, 0}, {_asm::field::Off4, 0x154}),
            _asm::field::Rt, _asm::field::Rn);

        R_UNLESS(ldrsw != nullptr, ldr::ResultInvalidDvcoRateConfig());

        /* Validate the str #0x2a0 */
        auto isCorrectStr = [](u32 w) {
            return _asm::IsOp(w, _asm::op::StrImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Off8)
                && _asm::Get(w, _asm::field::Off8) == 0x2a0;
        };
        R_UNLESS(isCorrectStr(ldrsw[1]) || isCorrectStr(ldrsw[2]), ldr::ResultInvalidDvcoRateConfig());

        /* Validate the target in both directuins */
        /* Due to codegen difference */
        u32 *add = nullptr;
        if (_asm::IsOp(ldrsw[-1], _asm::op::AddShifted64, _asm::field::Rd, _asm::field::Rn, _asm::field::Rm, _asm::field::Shift, _asm::field::Imm6)
            && _asm::Get(ldrsw[-1], _asm::field::Shift) == 0
            && _asm::Get(ldrsw[-1], _asm::field::Imm6) == 5) {
                add = ldrsw - 1;
            }
        else if (_asm::IsOp(ldrsw[1], _asm::op::AddShifted64, _asm::field::Rd, _asm::field::Rn, _asm::field::Rm, _asm::field::Shift, _asm::field::Imm6)
            && _asm::Get(ldrsw[1], _asm::field::Shift) == 0
            && _asm::Get(ldrsw[1], _asm::field::Imm6) == 5) {
                add = ldrsw + 1;
            }

        R_UNLESS(add != nullptr, ldr::ResultInvalidDvcoRateConfig());

        /* Patch the second target */
        PATCH_OFFSET(add, _asm::Encode(_asm::op::AddShifted64,
            {_asm::field::Rd, _asm::Get(*add, _asm::field::Rd)},
            {_asm::field::Rn, _asm::Get(*add, _asm::field::Rn)},
            {_asm::field::Rm, _asm::Get(*add, _asm::field::Rm)},
            {_asm::field::Shift, 0}, {_asm::field::Imm6, 6}));

        R_SUCCEED();
    }
}
