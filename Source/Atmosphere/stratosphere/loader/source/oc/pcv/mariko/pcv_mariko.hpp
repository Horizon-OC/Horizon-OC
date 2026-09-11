/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
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

#include "../../oc_common.hpp"
#include "../pcv_common.hpp"
#include "../pcv_asm.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    constexpr u32 BusFreqTableCount = 2;

    struct HookPayloadData {
        struct {
            u32 *next; /* Save the bytes. */
            uintptr_t originalFnCallback;
            u32 table[BusFreqTableCount][EmcDvfsTableEntryCount]; /* Original bus table size: 32. */
        } busData;
#if HOC_UART_LOG
        u32 verbosityLevel;
#endif
    };
    DECLARE_HOOK_PAYLOAD_PTR(HookPayloadData, m_HookPayloadData);

    void Patch(uintptr_t mapped_nso, size_t nso_size);

}
