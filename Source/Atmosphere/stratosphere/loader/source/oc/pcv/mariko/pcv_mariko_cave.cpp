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

#include "pcv_mariko_cave.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    namespace {

        struct {
            u32 *site = nullptr;
            u32 *end  = nullptr;
        } caveCache;

        bool HasDirectCaller(const u32 *target, const u32 *begin, const u32 *end) {
            for (const u32 *ptr = begin; ptr < end; ++ptr) {
                const u32 top6 = *ptr & 0xFC000000u;
                if (top6 != _asm::op::B && top6 != _asm::op::Bl) {
                    continue;
                }

                if (_asm::Target26(*ptr, reinterpret_cast<uintptr_t>(ptr)) == reinterpret_cast<uintptr_t>(target)) {
                    return true;
                }
            }

            return false;
        }

    }

    Result DeadDestructorCaveFind(u32 *ptr) {
        u32 *textEnd = reinterpret_cast<u32 *>(g_pcv_cave);

        u32 *site = _asm::FindFnPrologue(ptr, CavePrologueMargin, nsoStart);
        R_UNLESS(site != nullptr, ldr::ResultInvalidHookCave());

        u32 *end = _asm::FindFnEnd(site, textEnd);
        R_UNLESS(end != nullptr, ldr::ResultInvalidHookCave());

        /* Ensure this function is never called. */
        R_UNLESS(!HasDirectCaller(site, nsoStart, textEnd), ldr::ResultInvalidHookCave());

        caveCache.site = site;
        caveCache.end  = end;
        R_SUCCEED();
    }

    Result CaveInstall() {
        if (Hooks().CaveFree() >= HookContext::RequiredCaveSize()) {
            R_SUCCEED();
        }

        R_UNLESS(caveCache.site != nullptr, ldr::ResultInvalidHookCave());

        const uintptr_t start = util::AlignUp(reinterpret_cast<uintptr_t>(caveCache.site + 1), sizeof(u64));
        const uintptr_t end   = reinterpret_cast<uintptr_t>(caveCache.end);
        R_UNLESS(start < end && end - start >= HookContext::RequiredCaveSize(), ldr::ResultHookPayloadTooLarge());

        /* Insert a break in case this function gets called for whatever reason. */
        *caveCache.site = _asm::BrkIns;

        Hooks().SetCave(start, end - start);
        R_SUCCEED();
    }

}
