/*
 * Copyright (c) 2019-2025 CTCaer
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
 *
 */

#pragma once
#include "regs.hpp"

// NOTE: MMU won't allow access to gpu address space (smmu carveout issue?)
namespace bpmpMmu {

    constexpr u32 CacheBase = 0x50040000;

    constexpr u32 CacheConfig      = 0x000;
    constexpr u32 CfgEnableCache        = (1u << 0);
    constexpr u32 CfgForceWriteThrough  = (1u << 3);
    constexpr u32 CfgMmuTagModeShift    = 8;
    constexpr u32 TagModeParallel       = 0;
    constexpr u32 CfgTagChkAbrtOnErr    = (1u << 14);

    constexpr u32 MaintReq         = 0x028;
    constexpr u32 MaintReqWayBitmapAll  = (0xFu << 8);

    constexpr u32 IntClear         = 0x044;
    constexpr u32 IntRawEvent      = 0x048;
    constexpr u32 IntMaintDone          = (1u << 0);

    constexpr u32 MmuFallbackEntry = 0x0A0;
    constexpr u32 MmuShadowCopyMask = 0x0A4;

    constexpr u32 MmuCfg           = 0x0AC;
    constexpr u32 MmuCfgSeqEn           = (1u << 1);
    constexpr u32 MmuCfgTlbEn           = (1u << 2);
    constexpr u32 MmuCfgAbortStoreLast  = (1u << 4);

    constexpr u32 MmuCmd            = 0x0B0;
    constexpr u32 MmuCmdInit            = 1;
    constexpr u32 MmuCmdCopyShadow      = 2;

    constexpr u32 ShadowEntryBase = CacheBase + 0x400;

    constexpr u32 MmuEnCached = (1u << 0);
    constexpr u32 MmuEnExec   = (1u << 1);
    constexpr u32 MmuEnRead   = (1u << 2);
    constexpr u32 MmuEnWrite  = (1u << 3);

    constexpr u32 MaintCleanInvalidWay = 19;
    constexpr u32 MaintInvalidWay      = 18;

    constexpr u32 CacheLineSize = 0x20;

    constexpr u32 DramStart = 0x80000000;
    constexpr u32 IramBase  = 0x40000000;
    constexpr u32 IramEnd   = 0x4003FFFF;

    struct MmuEntry {
        u32 startAddr;
        u32 endAddr;
        u32 attr;
    };

    constexpr MmuEntry Entries[] = {
        { DramStart, 0xFFFFFFFF, MmuEnRead | MmuEnWrite | MmuEnExec | MmuEnCached },
        { IramBase,  IramEnd,    MmuEnRead | MmuEnWrite | MmuEnExec | MmuEnCached },
    };

    inline u32 AlignUp(u32 x, u32 a) {
        return (x + a - 1) & ~(a - 1);
    }
    inline u32 AlignDown(u32 x, u32 a) {
        return x & ~(a - 1);
    }

    inline void Maintenance(u32 op, bool force) {
        if (!force && !(MMIO32(CacheBase + CacheConfig) & CfgEnableCache)) {
            return;
        }

        MMIO32(CacheBase + IntClear) = IntMaintDone;

        // Blocking operation.
        MMIO32(CacheBase + MaintReq) = MaintReqWayBitmapAll | op;

        while (!(MMIO32(CacheBase + IntRawEvent) & IntMaintDone))
            ;

        MMIO32(CacheBase + IntClear) = MMIO32(CacheBase + IntRawEvent);
    }

    inline void SetEntry(u32 idx, const MmuEntry &entry) {
        MMIO32(ShadowEntryBase + sizeof(u32) * 4 * idx + 0x0) = AlignUp(entry.startAddr, CacheLineSize);
        MMIO32(ShadowEntryBase + sizeof(u32) * 4 * idx + 0x4) = AlignDown(entry.endAddr, CacheLineSize);
        MMIO32(ShadowEntryBase + sizeof(u32) * 4 * idx + 0x8) = entry.attr;

        MMIO32(CacheBase + MmuShadowCopyMask) |= (1u << idx);
    }

    inline void Enable() {
        if (MMIO32(CacheBase + CacheConfig) & CfgEnableCache) {
            return;
        }

        // Init BPMP MMU.
        MMIO32(CacheBase + MmuCmd)            = MmuCmdInit;
        MMIO32(CacheBase + MmuFallbackEntry)  = MmuEnRead | MmuEnWrite | MmuEnExec; // RWX for non-defined regions.
        MMIO32(CacheBase + MmuCfg)            = MmuCfgSeqEn | MmuCfgTlbEn | MmuCfgAbortStoreLast;

        // Init BPMP MMU entries.
        MMIO32(CacheBase + MmuShadowCopyMask) = 0;
        for (u32 idx = 0; idx < sizeof(Entries) / sizeof(Entries[0]); idx++) {
            SetEntry(idx, Entries[idx]);
        }

        MMIO32(CacheBase + MmuCmd) = MmuCmdCopyShadow;

        // Invalidate cache.
        Maintenance(MaintInvalidWay, true);

        // Enable cache.
        MMIO32(CacheBase + CacheConfig) = CfgEnableCache | CfgForceWriteThrough |
                                           (TagModeParallel << CfgMmuTagModeShift) | CfgTagChkAbrtOnErr;

        // HW bug. Invalidate cache again.
        Maintenance(MaintInvalidWay, false);
    }

    inline void Disable() {
        if (!(MMIO32(CacheBase + CacheConfig) & CfgEnableCache)) {
            return;
        }

        // Clean and invalidate cache.
        Maintenance(MaintCleanInvalidWay, false);

        // Disable cache.
        MMIO32(CacheBase + CacheConfig) = 0;
    }

} // namespace bpmpMmu
