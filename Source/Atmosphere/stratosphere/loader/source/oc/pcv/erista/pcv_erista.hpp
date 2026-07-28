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
#include "../pcv_hook.hpp"

namespace ams::ldr::hoc::pcv::erista {

    struct HookPayloadData {
        struct {
            EristaMtcTable *mtcTable;
            u32             mtcCount;
        } mtcTableAsm;
    };
    DECLARE_HOOK_PAYLOAD_PTR(HookPayloadData, e_HookPayloadData);

    extern u32 *nsoStart;

    void Patch(uintptr_t mapped_nso, size_t nso_size);

}
