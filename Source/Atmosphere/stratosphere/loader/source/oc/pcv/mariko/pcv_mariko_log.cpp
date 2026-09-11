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

#include "../pcv.hpp"
#include "pcv_mariko_log.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    /* Todo: Remove this bs. */
    [[maybe_unused]] static uintptr_t CaveReserve(size_t count) {
        return reinterpret_cast<uintptr_t>(Hooks().Reserve(count));
    }

    #if HOC_UART_LOG
    namespace {
        struct {
            uintptr_t vsnprintf_addr = 0;
            uintptr_t nvlog_addr     = 0;
        } nvLogCache;
    }

    Result NvLogUartRedirect(u32 *ptr) {
        const uintptr_t mapped_nso     = reinterpret_cast<uintptr_t>(nsoStart);
        const size_t    nso_size       = g_nso_size;
        const uintptr_t textEnd        = g_pcv_cave; /* .text ends where the cave begins */
        const uintptr_t vsnprintf_addr = reinterpret_cast<uintptr_t>(ptr);

        {
            using namespace _asm::field;
            R_UNLESS(_asm::Ignoring(ptr[1], NvLogStpFpLrAsm, PairOff8),  ldr::ResultInvalidNvLogRedirect()); /* stp x29,x30,[sp,#imm] */
            R_UNLESS(_asm::Ignoring(ptr[2], NvLogStrSpillAsm, Rt, Off8), ldr::ResultInvalidNvLogRedirect()); /* str x?,[sp,#imm]      */
            R_UNLESS(_asm::Ignoring(ptr[3], NvLogMovFpAsm, Imm12),       ldr::ResultInvalidNvLogRedirect()); /* add x29,sp,#imm       */
            R_UNLESS(ptr[4] == NvLogCmpSizeAsm,                          ldr::ResultInvalidNvLogRedirect()); /* cmp x1,#0             */
        }

        /* NvLog via the VDD_SOC log */
        static const char Fmt[] = "%s(%s): DVFS request VDD_SOC %d mV\n";
        constexpr size_t FmtLen = sizeof(Fmt) - 1;
        uintptr_t strAddr = 0;
        {
            const char *hay = reinterpret_cast<const char *>(mapped_nso);
            for (size_t i = 0; i + FmtLen <= nso_size; ++i) {
                if (std::memcmp(hay + i, Fmt, FmtLen) == 0) { strAddr = mapped_nso + i; break; }
            }
        }
        if (strAddr == 0) {
            LOGGING("NvLogRedirect: fmt string not found (vsnprintf@+%lx)", vsnprintf_addr - mapped_nso);
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }

        uintptr_t nvlog_addr = 0;
        for (u32 *p = nsoStart; reinterpret_cast<uintptr_t>(p + 2) <= textEnd; ++p) {
            const uintptr_t pc = reinterpret_cast<uintptr_t>(p);
            if (!_asm::IsOp(p[0], _asm::op::Adrp, _asm::field::Rd, _asm::field::ImmAdrpHi, _asm::field::ImmAdrpLo)) {
                continue;
            }
            if (_asm::TargetAdrp(p[0], pc) != (strAddr & ~static_cast<uintptr_t>(0xFFFu))) {
                continue;
            }
            const u32 adrpReg = _asm::Get(p[0], _asm::field::Rd);
            if (!(_asm::IsOp(p[1], _asm::op::AddImm64, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12)
                  && _asm::Get(p[1], _asm::field::Rd) == adrpReg && _asm::Get(p[1], _asm::field::Rn) == adrpReg
                  && _asm::Get(p[1], _asm::field::Imm12) == (strAddr & 0xFFFu))) {
                continue;
            }
            for (u32 k = 2; k <= 12 && (pc + (k + 1) * 4) <= textEnd; ++k) {
                if (_asm::IsOp(p[k], _asm::op::Bl, _asm::field::Imm26)) { nvlog_addr = _asm::Target26(p[k], pc + k * 4); break; }
            }
            if (nvlog_addr != 0) {
                break;
            }
        }
        if (nvlog_addr == 0 || nvlog_addr < mapped_nso || nvlog_addr >= textEnd) {
            LOGGING("NvLogRedirect: NvLog entry not found (fmt@+%lx)", strAddr - mapped_nso);
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }

        nvLogCache.vsnprintf_addr = vsnprintf_addr;
        nvLogCache.nvlog_addr     = nvlog_addr;
        R_SUCCEED();
    }

    namespace {
        Result InstallNvLogRedirect() {
            const uintptr_t mapped_nso     = reinterpret_cast<uintptr_t>(nsoStart);
            const uintptr_t textEnd        = g_pcv_cave;
            const uintptr_t vsnprintf_addr = nvLogCache.vsnprintf_addr;
            const uintptr_t nvlog_addr     = nvLogCache.nvlog_addr;

            const uintptr_t helper = CaveReserve(40);
            if (helper == 0) {
                LOGGING("NvLogRedirect: cave unavailable (cave=%lx size=%lx)",
                        static_cast<unsigned long>(g_pcv_cave), static_cast<unsigned long>(g_pcv_cave_size));
                R_THROW(ldr::ResultInvalidNvLogRedirect());
            }
            u32 *t = reinterpret_cast<u32 *>(helper);
            size_t n = 0;
            auto emit = [&](u32 ins) { t[n] = ins; ++n; };

            using namespace _asm::field;
            constexpr u32 SP = _asm::reg::Sp;

            emit(_asm::Encode(_asm::op::SubImm64, {Rd, SP}, {Rn, SP}, {Imm12, 0x200}));
            emit(_asm::Encode(_asm::op::StpImm64, {Rt, 0}, {Rt2, 1}, {Rn, SP}, {PairOff8, 0x100}));
            emit(_asm::Encode(_asm::op::StpImm64, {Rt, 2}, {Rt2, 3}, {Rn, SP}, {PairOff8, 0x110}));
            emit(_asm::Encode(_asm::op::StpImm64, {Rt, 4}, {Rt2, 5}, {Rn, SP}, {PairOff8, 0x120}));
            emit(_asm::Encode(_asm::op::StpImm64, {Rt, 6}, {Rt2, 7}, {Rn, SP}, {PairOff8, 0x130}));
            emit(_asm::Encode(_asm::op::StpImmQ,  {Rt, 0}, {Rt2, 1}, {Rn, SP}, {PairOff16, 0x140}));
            emit(_asm::Encode(_asm::op::StpImmQ,  {Rt, 2}, {Rt2, 3}, {Rn, SP}, {PairOff16, 0x160}));
            emit(_asm::Encode(_asm::op::StpImmQ,  {Rt, 4}, {Rt2, 5}, {Rn, SP}, {PairOff16, 0x180}));
            emit(_asm::Encode(_asm::op::StpImmQ,  {Rt, 6}, {Rt2, 7}, {Rn, SP}, {PairOff16, 0x1A0}));
            emit(_asm::Encode(_asm::op::StrImm64, {Rt, _asm::reg::Lr}, {Rn, SP}, {Off8, 0x1E0}));
            emit(_asm::Encode(_asm::op::AddImm64, {Rd, 9}, {Rn, SP}, {Imm12, 0x200})); emit(_asm::Encode(_asm::op::StrImm64, {Rt, 9}, {Rn, SP}, {Off8, 0x1C0})); /* __stack  */
            emit(_asm::Encode(_asm::op::AddImm64, {Rd, 9}, {Rn, SP}, {Imm12, 0x140})); emit(_asm::Encode(_asm::op::StrImm64, {Rt, 9}, {Rn, SP}, {Off8, 0x1C8})); /* __gr_top */
            emit(_asm::Encode(_asm::op::AddImm64, {Rd, 9}, {Rn, SP}, {Imm12, 0x1C0})); emit(_asm::Encode(_asm::op::StrImm64, {Rt, 9}, {Rn, SP}, {Off8, 0x1D0})); /* __vr_top */
            emit(_asm::Encode(_asm::op::MovnW, {Rd, 9}, {Imm16, 0x37})); emit(_asm::Encode(_asm::op::StrImm32, {Rt, 9}, {Rn, SP}, {Off4, 0x1D8})); /* __gr_offs = -56  */
            emit(_asm::Encode(_asm::op::MovnW, {Rd, 9}, {Imm16, 0x7F})); emit(_asm::Encode(_asm::op::StrImm32, {Rt, 9}, {Rn, SP}, {Off4, 0x1DC})); /* __vr_offs = -128 */
            emit(_asm::Encode(_asm::op::AddImm64, {Rd, 0}, {Rn, SP}, {Imm12, 0x00})); /* mov x0,sp (buf)  */
            emit(_asm::Encode(_asm::op::MovzW, {Rd, 1}, {Imm16, 0x100}));             /* size = 0x100     */
            emit(_asm::Encode(_asm::op::LdrImm64, {Rt, 2}, {Rn, SP}, {Off8, 0x100})); /* fmt (saved x0)   */
            emit(_asm::Encode(_asm::op::AddImm64, {Rd, 3}, {Rn, SP}, {Imm12, 0x1C0})); /* ap              */
            emit(_asm::Retarget26(_asm::op::Bl, helper + n * 4, vsnprintf_addr));
            emit(_asm::MovReg64(1, 0)); /* len = retval     */
            emit(_asm::CmpImm32(1, 0x100));
            { /* b.lo +2 */
                const size_t at = n;
                emit(_asm::Retarget19(_asm::Encode(_asm::op::BCond, {BCond, _asm::cond::Lo}), helper + at * 4, helper + (at + 2) * 4));
            }
            emit(_asm::Encode(_asm::op::MovzW, {Rd, 1}, {Imm16, 0xFF}));              /* clamp len        */
            emit(_asm::Encode(_asm::op::AddImm64, {Rd, 0}, {Rn, SP}, {Imm12, 0x00})); /* mov x0,sp (str)  */
            emit(_asm::Encode(_asm::op::Svc, {Imm16, 0x27}));                         /* svcOutputDebugString */
            emit(_asm::Encode(_asm::op::LdrImm64, {Rt, _asm::reg::Lr}, {Rn, SP}, {Off8, 0x1E0}));
            emit(_asm::Encode(_asm::op::AddImm64, {Rd, SP}, {Rn, SP}, {Imm12, 0x200}));
            emit(_asm::RetIns);

            /* Redirect the call sites as patching the actual function causes crash */
            const uintptr_t roStart = g_pcv_cave + g_pcv_cave_size; /* module .rodata start */
            size_t patchedSites = 0;
            for (u32 *p = nsoStart; reinterpret_cast<uintptr_t>(p + 1) <= textEnd; ++p) {
                if (!_asm::IsOp(*p, _asm::op::Bl, _asm::field::Imm26)) {
                    continue;
                }
                const uintptr_t pc = reinterpret_cast<uintptr_t>(p);
                if (_asm::Target26(*p, pc) != nvlog_addr) {
                    continue;
                }
                bool isFmtCall = false;
                for (u32 j = 1; j <= 8 && reinterpret_cast<uintptr_t>(p - j) >= reinterpret_cast<uintptr_t>(nsoStart); ++j) {
                    const u32 w = *(p - j);
                    if (_asm::IsOp(w, _asm::op::Adrp, _asm::field::Rd, _asm::field::ImmAdrpHi, _asm::field::ImmAdrpLo) && _asm::Get(w, _asm::field::Rd) == 0) { /* adrp x0,<page> */
                        const uintptr_t tgtPage = _asm::TargetAdrp(w, pc - j * 4);
                        if (tgtPage >= (roStart & ~static_cast<uintptr_t>(0xFFFu))) { isFmtCall = true; break; }
                    }
                }
                if (isFmtCall) {
                    PATCH_OFFSET(p, _asm::Retarget26(_asm::op::Bl, pc, helper));
                    ++patchedSites;
                }
            }

            LOGGING("NvLogRedirect: stub@+%lx vsnprintf@+%lx helper@+%lx instr=%zu sites=%zu", nvlog_addr - mapped_nso, vsnprintf_addr - mapped_nso, helper - mapped_nso, n, patchedSites);
            R_SUCCEED();
        }
    }

    namespace {
        struct {
            u32 *sites[3] = {};
            u32  count    = 0;
        } forceVerbosityCache;
    }

    HOOK_PAYLOAD_FN u32 ForceVerbosityImpl() {
        HookPayloadData *data = HOOK_PAYLOAD_PTR(HookPayloadData, m_HookPayloadData);
        return data->verbosityLevel;
    }

    /* Force GetEffectiveVerbosityLevel to return a non-zero level so all NvLog runs. */
    Result ForceVerbosity(u32 *ptr) {
        using namespace _asm::field;

        R_UNLESS(_asm::IsOp(ptr[1], _asm::op::StrImm64, Rt, Rn, Off8) && _asm::Get(ptr[1], Rn) == _asm::reg::Sp,
                 ldr::ResultInvalidForceVerbosityPattern()); /* str x?,[sp,#imm] */
        R_UNLESS(_asm::IsOp(ptr[2], _asm::op::AddImm64, Rd, Rn, Imm12)
                 && _asm::Get(ptr[2], Rd) == _asm::reg::Fp && _asm::Get(ptr[2], Rn) == _asm::reg::Sp,
                 ldr::ResultInvalidForceVerbosityPattern()); /* add x29,sp,#imm  */
        R_UNLESS(_asm::IsOp(ptr[3], _asm::op::AddImm64, Rn, Imm12)
                 && _asm::Get(ptr[3], Rd) == 0 && _asm::Get(ptr[3], Rn) == _asm::reg::Fp,
                 ldr::ResultInvalidForceVerbosityPattern()); /* add x0,x29,#imm  */
        R_UNLESS(_asm::IsOp(ptr[4], _asm::op::AddImm64, Rd, Rn, Imm12) && _asm::Get(ptr[4], Rn) == _asm::reg::Fp,
                 ldr::ResultInvalidForceVerbosityPattern()); /* add x?,x29,#imm */
        R_UNLESS(_asm::Get(ptr[3], Imm12) == _asm::Get(ptr[4], Imm12)
                 && _asm::IsOp(ptr[5], _asm::op::Bl, Imm26),
                 ldr::ResultInvalidForceVerbosityPattern());

        bool foundCmp = false;
        for (u32 j = 6; j <= 10; ++j) {
            if (ptr[j] == 0x7100001Fu) { /* cmp w0,#0 */
                foundCmp = true;
                break;
            }
        }
        R_UNLESS(foundCmp, ldr::ResultInvalidForceVerbosityPattern());

        R_UNLESS(forceVerbosityCache.count < std::size(forceVerbosityCache.sites), ldr::ResultInvalidForceVerbosityPattern());
        forceVerbosityCache.sites[forceVerbosityCache.count++] = ptr;
        R_SUCCEED();
    }

    Result ForceVebosityInstallHooks(HookPayloadData *data) {
        if (nvLogCache.nvlog_addr != 0) {
            R_TRY(InstallNvLogRedirect());
        }

        if (forceVerbosityCache.count != 0 && C.pcvLogVerbosity != 0xff) {
            data->verbosityLevel = C.pcvLogVerbosity;
            for (u32 i = 0; i < forceVerbosityCache.count; ++i) {
                R_DISCARD(INSTALL_IMPL_HOOK(forceVerbosityCache.sites[i], ForceVerbosityImpl));
            }
        }
    }
#endif

}
