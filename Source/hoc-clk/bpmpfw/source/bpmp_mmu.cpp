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

#include "bpmp_mmu.hpp"

namespace bpmpMmu {

    u32 AlignUp(u32 x, u32 a) {
        return (x + a - 1) & ~(a - 1);
    }

    u32 AlignDown(u32 x, u32 a) {
        return x & ~(a - 1);
    }

    void Maintenance(u32 op, bool force) {
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

    void SetEntry(u32 idx, const MmuEntry &entry) {
        MMIO32(ShadowEntryBase + sizeof(u32) * 4 * idx + 0x0) = AlignUp(entry.startAddr, CacheLineSize);
        MMIO32(ShadowEntryBase + sizeof(u32) * 4 * idx + 0x4) = AlignDown(entry.endAddr, CacheLineSize);
        MMIO32(ShadowEntryBase + sizeof(u32) * 4 * idx + 0x8) = entry.attr;

        MMIO32(CacheBase + MmuShadowCopyMask) |= (1u << idx);
    }

    void Enable() {
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

    void Disable() {
        if (!(MMIO32(CacheBase + CacheConfig) & CfgEnableCache)) {
            return;
        }

        // Clean and invalidate cache.
        Maintenance(MaintCleanInvalidWay, false);

        // Disable cache.
        MMIO32(CacheBase + CacheConfig) = 0;
    }

} // namespace bpmpMmu
