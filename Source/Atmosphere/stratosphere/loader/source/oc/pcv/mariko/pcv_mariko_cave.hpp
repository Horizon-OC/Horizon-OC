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
 */

#pragma once

#include "../pcv.hpp"
#include "../pcv_asm.hpp"
#include "../pcv_hook.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    extern u32 *nsoStart;

    constexpr u32 CaveFlagChunkCount    = 4;
    constexpr u32 CaveFlagChunkInsCount = 4;
    constexpr u32 CaveFlagLdrbDelta     = 0x30;
    constexpr u32 CavePrologueMargin    = 8;

    /* This function consists of multiple chunks. */
    /* Each chunk does a load, a comparison and a conditional branch. The next chunk's address setup (adrp) follows right after it. */
    inline bool IsFlagCheckChunk(const u32 *ptr) {
        return _asm::IsOp(ptr[0], _asm::op::LdrbImm, _asm::field::Rt, _asm::field::Rn, _asm::field::Off1)
            && _asm::Ignoring(ptr[1], _asm::CmpImm32(0, 1), _asm::field::Rn)
            && _asm::Ignoring(ptr[2], _asm::Encode(_asm::op::BCond, {_asm::field::BCond, _asm::cond::Eq}), _asm::field::Imm19);
    }

    inline bool IsChunkDeltaValid(const u32 *prev, const u32 *next) {
        return IsFlagCheckChunk(next) && _asm::Get(*next, _asm::field::Off1) == _asm::Get(*prev, _asm::field::Off1) + CaveFlagLdrbDelta;
    }

    inline bool ValidateChunkConsistency(const u32 *first) {
        for (u32 i = 1; i < CaveFlagChunkCount; ++i) {
            if (!IsChunkDeltaValid(first + (i - 1) * CaveFlagChunkInsCount, first + i * CaveFlagChunkInsCount)) {
                return false;
            }
        }

        return true;
    }

    inline bool CaveFlagRunPatternFn(u32 *ptr) {
        if (Hooks().CaveFree() >= HookContext::RequiredCaveSize()) {
            return false;
        }
        /* In case we don't have enough cave memory, we need to commit this warcrime. */

        /* Ensure we don't dereference invalid memory. */
        u32 *textEnd = reinterpret_cast<u32 *>(g_pcv_cave);
        if (ptr - CaveFlagChunkInsCount < nsoStart || ptr + CaveFlagChunkCount * CaveFlagChunkInsCount > textEnd) {
            return false;
        }

        /* Validate the current chunk and the previous chunk and ensure the delta of the ldrb instructions is consistent. */
        if (!IsFlagCheckChunk(ptr) || IsChunkDeltaValid(ptr - CaveFlagChunkInsCount, ptr)) {
            return false;
        }

        return ValidateChunkConsistency(ptr);
    }

    Result DeadDestructorCaveFind(u32 *ptr);
    Result CaveInstall();

}
