/*
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

#pragma once

#include "../pcv.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    extern u32 *nsoStart;
    extern size_t g_nso_size;

    inline bool ForceVerbosityPatternFn(u32 *ptr) {
        if (g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 11) > g_pcv_cave) {   /* .text only */
            return false;
        }
        return _asm::IsFramePushPre(*ptr);
    }

    /*
        sub  sp,  sp, #imm
        stp  x29,x30,[sp,#imm]
        str  x?, [sp,#imm]
        add  x29,sp, #imm
        subs xzr, x1, #0
    */
    constexpr u32 NvLogSubSpAsm    = 0xD10003FFu; /* sub  sp,sp,#0 */
    constexpr u32 NvLogStpFpLrAsm  = 0xA9007BFDu; /* stp  x29,x30,[sp,#0] */
    constexpr u32 NvLogStrSpillAsm = 0xF90003E0u; /* str  x?,[sp,#0] */
    constexpr u32 NvLogMovFpAsm    = 0x910003FDu; /* add  x29,sp,#0 */
    constexpr u32 NvLogCmpSizeAsm  = 0xF100003Fu; /* subs xzr,x1,#0 */

    inline bool NvLogVsnprintfPatternFn(u32 *ptr) {
        if (HOC_UART_LOG == 0 || g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 5) > g_pcv_cave) {   /* must sit in .text */
            return false;
        }
        return _asm::Ignoring(*ptr, NvLogSubSpAsm, _asm::field::Imm12);
    }

    Result NvLogUartRedirect(u32 *ptr);
    HOOK_PAYLOAD_FN u32 ForceVerbosityImpl();
    Result ForceVerbosity(u32 *ptr);
    Result ForceVebosityInstallHooks(HookPayloadData *data);

}
