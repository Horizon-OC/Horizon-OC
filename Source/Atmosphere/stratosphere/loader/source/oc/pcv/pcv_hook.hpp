/*
 * Copyright (c) Lightos_
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

#include "../oc_common.hpp"

#define HOOK_PAYLOAD_FN __attribute__((section("hoc_hookpayload"), used, noinline, visibility("hidden")))

/* Inline asm because fuck compilers: GCC refuses to put a variable and a function in the same section. */
/* It wants alloc+write for one and alloc+exec for the other.                                           */
/* You are supposed to be able to override that by writing the flags into the section name yourself,    */
/* like section("hoc_hookpayload,\"ax\",%progbits"), instead of letting GCC pick them.                  */
/* But GCC takes that whole string as the name and never reads the flags out of it, so it appends the   */
/* ones it wanted anyway: the .section directive it emits ends up carrying two sets of flags, and the   */
/* assembler rejects it. The failure lands in the generated assembly, not in anything we wrote.         */
/* Therefore we must use inline assembly, which reaches the assembler exactly as written.               */
#define DEFINE_HOOK_PAYLOAD_PTR(type, name)                     \
    asm(".section hoc_hookpayload,\"ax\",%progbits\n"           \
        ".balign 8\n"                                           \
        ".global " #name "\n"                                   \
        ".hidden " #name "\n"                                   \
        #name ": .zero 8\n"                                     \
        ".text\n");                                             \
    extern "C" __attribute__((visibility("hidden"))) type *name

#define DECLARE_HOOK_PAYLOAD_PTR(type, name)                    \
    extern "C" __attribute__((visibility("hidden"))) type *name

#define HOOK_PAYLOAD_PTR(type, name)                            \
    ([]() -> type * {                                           \
        type **_hoc_pp;                                         \
        __asm__("adr %0, " #name : "=r"(_hoc_pp));              \
        return *_hoc_pp;                                        \
    }())

extern "C" const u8 __start_hoc_hookpayload[];
extern "C" const u8 __stop_hoc_hookpayload[];

namespace ams::ldr::hoc::pcv {

    constexpr size_t HookPageSize     = 0x1000;
    constexpr size_t PcvDataArenaSize = 0x1000;

    inline s64 SignExtend(u64 value, int bits) {
        const int shift = 64 - bits;
        return static_cast<s64>(value << shift) >> shift;
    }

    inline u32 EncodeRelBranch(u32 opc, uintptr_t site_va, uintptr_t target_va) {
        const s64 delta = static_cast<s64>(target_va) - static_cast<s64>(site_va);
        AMS_ABORT_UNLESS((delta & 0x3) == 0);
        AMS_ABORT_UNLESS(delta >= -0x08000000 && delta <= 0x07FFFFFC);
        return opc | (static_cast<u32>(delta >> 2) & 0x03FFFFFFu);
    }

    inline u32 EncodeB(uintptr_t site_va, uintptr_t target_va)  { return EncodeRelBranch(0x14000000u, site_va, target_va); }
    inline u32 EncodeBL(uintptr_t site_va, uintptr_t target_va) { return EncodeRelBranch(0x94000000u, site_va, target_va); }

    inline u32 EncodePairSp(bool load, u32 rt1, u32 rt2, s32 imm) {
        const u32 base = load ? 0xA9400000u : 0xA9000000u;
        const u32 imm7 = static_cast<u32>((imm / 8) & 0x7F);
        return base | (imm7 << 15) | (rt2 << 10) | (31u << 5) | rt1;
    }

    inline u32 EncodeSpAdjust(bool sub, u32 imm12) {
        const u32 base = sub ? 0xD1000000u : 0x91000000u;
        return base | ((imm12 & 0xFFFu) << 10) | (31u << 5) | 31u;
    }

    inline Result RelocateInstruction(u32 insn, uintptr_t old_site_va, uintptr_t new_site_va, u32 *out) {
        const u32 top6 = insn & 0xFC000000u;
        if (top6 == 0x14000000u || top6 == 0x94000000u) {
            const s64 old_imm      = SignExtend(static_cast<u64>(insn & 0x03FFFFFFu) << 2, 28);
            const uintptr_t target = old_site_va + old_imm;
            *out = EncodeRelBranch(top6, new_site_va, target);
            R_SUCCEED();
        }

        R_UNLESS((insn & 0xFF000010u) != 0x54000000u, ldr::ResultHookRelocationUnsupported()); /* b.cond      */
        R_UNLESS((insn & 0x7E000000u) != 0x34000000u, ldr::ResultHookRelocationUnsupported()); /* cbz/cbnz    */
        R_UNLESS((insn & 0x7E000000u) != 0x36000000u, ldr::ResultHookRelocationUnsupported()); /* tbz/tbnz    */
        R_UNLESS((insn & 0x1F000000u) != 0x10000000u, ldr::ResultHookRelocationUnsupported()); /* adr/adrp    */
        R_UNLESS((insn & 0x3B000000u) != 0x18000000u, ldr::ResultHookRelocationUnsupported()); /* ldr literal */

        *out = insn;
        R_SUCCEED();
    }

    class HookContext {
        private:
            uintptr_t m_map_base   = 0; /* loader-side base of pcv's NSO mapping.    */
            uintptr_t m_va_base    = 0; /* pcv-side base of the same memory.         */
            uintptr_t m_cave       = 0; /* loader-side cave base. (0 if unavailable) */
            size_t    m_cave_size  = 0;
            uintptr_t m_payload    = 0; /* loader-side base of the payload copy. */
            size_t    m_payload_sz = 0;
            size_t    m_used       = 0;
            uintptr_t m_data       = 0; /* loader-side data arena base, 0 if none. */
            size_t    m_data_used  = 0;
        public:
            constexpr HookContext() = default;

            void Initialize(uintptr_t map_base, uintptr_t va_base, uintptr_t cave, size_t cave_size, uintptr_t data) {
                m_map_base   = map_base;
                m_va_base    = va_base;
                m_cave       = cave;
                m_cave_size  = cave_size;
                m_payload    = 0;
                m_payload_sz = 0;
                m_used       = 0;
                m_data       = data;
                m_data_used  = 0;
            }

            bool IsEnabled() const { return m_cave != 0 && m_cave_size != 0; }

            Result CheckEnabled() const {
                R_UNLESS(this->IsEnabled(), ldr::ResultHookUnavailable());
                R_SUCCEED();
            }

            size_t CaveSize() const { return m_cave_size; }
            size_t CaveUsed() const { return m_used; }
            size_t CaveFree() const { return m_cave_size > m_used ? m_cave_size - m_used : 0; }

            size_t DataSize() const { return m_data != 0 ? PcvDataArenaSize : 0; }
            size_t DataUsed() const { return m_data_used; }
            size_t DataFree() const { return this->DataSize() - m_data_used; }

            /* Convert loader mapped address to pcv-side address. */
            uintptr_t ToVa(const void *loader_ptr) const {
                return m_va_base + (reinterpret_cast<uintptr_t>(loader_ptr) - m_map_base);
            }

            uintptr_t CaveVa() const { return ToVa(reinterpret_cast<const void *>(m_cave)); }

            Result CopyPayload() {
                R_TRY(this->CheckEnabled());

                const size_t size = static_cast<size_t>(__stop_hoc_hookpayload - __start_hoc_hookpayload);

                /*  Zero length: Linker garbage collected the payload .(happens when nothing references it) */
                /* __start_ and _stop_ don't prevent this. */
                /* Copying here would succeed at first but fail later by jumping to empty memory. */
                R_UNLESS(size != 0,           ldr::ResultUninitializedPatcher());
                R_UNLESS(size <= m_cave_size, ldr::ResultHookPayloadTooLarge());

                m_payload    = m_cave;
                m_payload_sz = size;
                std::memcpy(reinterpret_cast<void *>(m_payload), __start_hoc_hookpayload, size);

                m_used = util::AlignUp(size, sizeof(u32));
                R_SUCCEED();
            }

            /* pcv-side address of a payload symbol's copy. */
            uintptr_t PayloadVa(const void *loader_sym) const {
                const uintptr_t offset = reinterpret_cast<uintptr_t>(loader_sym) - reinterpret_cast<uintptr_t>(__start_hoc_hookpayload);
                return this->ToVa(reinterpret_cast<void *>(m_payload + offset));
            }

            /* Loader-side, writable pointer to a payload variable's copy in the cave. */
            template<typename T>
            T *PayloadCopyOf(T &loader_sym) const {
                const uintptr_t offset = reinterpret_cast<uintptr_t>(std::addressof(loader_sym)) - reinterpret_cast<uintptr_t>(__start_hoc_hookpayload);
                return reinterpret_cast<T *>(m_payload + offset);
            }

            /* Reserves space in the writable data arena past pcv's .bss. */
            /* Returns a zeroed, loader-side pointer. */
            /* The data arena is too far from the cave to be addressed directly by symbol, so we must store a pointer to it in the cave section. */
            template<typename T>
            T *DataAlloc() {
                const size_t size = util::AlignUp(sizeof(T), alignof(T) > 8 ? alignof(T) : 8);
                if (m_data == 0 || m_data_used + size > PcvDataArenaSize) {
                    return nullptr;
                }

                T *p = reinterpret_cast<T *>(m_data + m_data_used);
                m_data_used += size;

                std::memset(p, 0, sizeof(T));
                return p;
            }

            template<typename T>
            T *BindData(T *&loader_sym) {
                T *block = this->DataAlloc<T>();
                if (block != nullptr) {
                    *this->PayloadCopyOf(loader_sym) = reinterpret_cast<T *>(this->ToVa(block));
                }
                return block;
            }

            /* Replaces the function entirely starting at function prologue. */
            /* Preserves the function arguments. */
            Result InstallImpl(u32 *site, const void *fn, uintptr_t *out_orig = nullptr) {
                R_TRY(this->CheckEnabled());
                R_UNLESS(site != nullptr,   ldr::ResultHookSiteInvalid());
                R_UNLESS(m_payload != 0,    ldr::ResultUninitializedPatcher());
                R_TRY(this->ValidatePayloadFn(fn));

                if (out_orig != nullptr) {
                    u32 *tramp = this->AllocCode(2);
                    R_UNLESS(tramp != nullptr, ldr::ResultHookArenaOutOfMemory());

                    u32 relocated;
                    R_TRY(RelocateInstruction(site[0], this->ToVa(site), this->ToVa(&tramp[0]), std::addressof(relocated)));
                    tramp[0] = relocated;
                    tramp[1] = EncodeB(this->ToVa(&tramp[1]), this->ToVa(site) + sizeof(u32));

                    *out_orig = this->ToVa(tramp);
                }

                site[0] = EncodeB(this->ToVa(site), this->PayloadVa(fn));

                R_SUCCEED();
            }

            /* Takes the same arguments as the hooked function, does not replace. */
            Result InstallIntercept(u32 *site, const void *fn) {
                R_TRY(this->CheckEnabled());
                R_UNLESS(site != nullptr, ldr::ResultHookSiteInvalid());
                R_UNLESS(m_payload != 0,  ldr::ResultUninitializedPatcher());
                R_TRY(this->ValidatePayloadFn(fn));

                /* x0-x7: arguments */
                /* x8: result pointer */
                /* x18: platform register */
                /* x30: return address */
                /* x29: keeps pairs clean */
                /* x9-x17: scratch */
                constexpr u32 FrameSize = 0x60;
                constexpr u32 Pairs[][2] = { {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 18}, {29, 30} };
                constexpr u32 PairCount  = sizeof(Pairs) / sizeof(Pairs[0]);
                constexpr u32 StubWords  = 1 + PairCount + 1 + PairCount + 1 + 1 + 1;

                u32 *stub = this->AllocCode(StubWords);
                R_UNLESS(stub != nullptr, ldr::ResultHookArenaOutOfMemory());

                u32 i = 0;
                stub[i++] = EncodeSpAdjust(true, FrameSize);
                for (u32 p = 0; p < PairCount; ++p) {
                    stub[i++] = EncodePairSp(false, Pairs[p][0], Pairs[p][1], static_cast<s32>(p * 16));
                }

                stub[i] = EncodeBL(this->ToVa(&stub[i]), this->PayloadVa(fn));
                ++i;

                for (u32 p = 0; p < PairCount; ++p) {
                    stub[i++] = EncodePairSp(true, Pairs[p][0], Pairs[p][1], static_cast<s32>(p * 16));
                }
                stub[i++] = EncodeSpAdjust(false, FrameSize);

                u32 relocated;
                R_TRY(RelocateInstruction(site[0], this->ToVa(site), this->ToVa(&stub[i]), std::addressof(relocated)));
                stub[i] = relocated;
                ++i;

                stub[i] = EncodeB(this->ToVa(&stub[i]), this->ToVa(site) + sizeof(u32));
                ++i;

                AMS_ABORT_UNLESS(i == StubWords);

                site[0] = EncodeB(this->ToVa(site), this->ToVa(stub));
                R_SUCCEED();
            }

            /* Checks entry to first ret that everything still points to valid data. */
            Result ValidatePayloadFn(const void *fn) const {
                constexpr u32 MaxInstructions = 512;
                constexpr u32 RetInsn         = 0xD65F03C0u;

                const uintptr_t lo = m_payload;
                const uintptr_t hi = m_payload + m_payload_sz;

                const u32 *insns = reinterpret_cast<const u32 *>(fn);
                const uintptr_t base = m_payload + (reinterpret_cast<uintptr_t>(fn) - reinterpret_cast<uintptr_t>(__start_hoc_hookpayload));

                auto check = [&](uintptr_t target) -> Result {
                    if (target < lo || target >= hi) {
                        R_THROW(ldr::ResultHookPayloadEscapes());
                    }
                    R_SUCCEED();
                };

                for (u32 i = 0; i < MaxInstructions; ++i) {
                    const u32 insn      = insns[i];
                    const uintptr_t site = base + i * sizeof(u32);

                    if (insn == RetInsn) {
                        R_SUCCEED();
                    }

                    const u32 top6 = insn & 0xFC000000u;
                    if (top6 == 0x14000000u || top6 == 0x94000000u) {                        /* b / bl      */
                        const uintptr_t target = site + SignExtend(static_cast<u64>(insn & 0x03FFFFFFu) << 2, 28);
                        R_TRY(check(target));
                    } else if ((insn & 0xFF000010u) == 0x54000000u ||                        /* b.cond      */
                               (insn & 0x7E000000u) == 0x34000000u ||                        /* cbz / cbnz  */
                               (insn & 0x3B000000u) == 0x18000000u) {                        /* ldr literal */
                        const uintptr_t target = site + SignExtend(static_cast<u64>((insn >> 5) & 0x7FFFFu) << 2, 21);
                        R_TRY(check(target));
                    } else if ((insn & 0x7E000000u) == 0x36000000u) {                        /* tbz / tbnz  */
                        const uintptr_t target = site + SignExtend(static_cast<u64>((insn >> 5) & 0x3FFFu) << 2, 16);
                        R_TRY(check(target));
                    } else if ((insn & 0x9F000000u) == 0x10000000u) {                        /* adr         */
                        const u64 imm          = (static_cast<u64>((insn >> 5) & 0x7FFFFu) << 2) | ((insn >> 29) & 0x3u);
                        const uintptr_t target = site + SignExtend(imm, 21);
                        R_TRY(check(target));
                    } else if ((insn & 0x9F000000u) == 0x90000000u) {                        /* adrp        */
                        /* ADRP is page-relative and the cave is at an arbitrary offset, any adrp would point to potential garbage. */
                        R_THROW(ldr::ResultHookPayloadEscapes());
                    }
                }

                /* No return found. */
                R_THROW(ldr::ResultHookPayloadEscapes());
            }
        private:
            u32 *AllocCode(size_t words) {
                const size_t bytes = words * sizeof(u32);
                if (!this->IsEnabled() || m_used + bytes > m_cave_size) {
                    return nullptr;
                }

                u32 *p = reinterpret_cast<u32 *>(m_cave + m_used);
                m_used += bytes;
                return p;
            }
    };

    inline HookContext &Hooks() {
        static HookContext s_context;
        return s_context;
    }

}

/* Hook custom impl. Original becomes unreachable. */
/* Starts at function entry. Preserves arguments. */
#define INSTALL_IMPL_HOOK(site, fn) \
    (::ams::ldr::hoc::pcv::Hooks().InstallImpl((site), reinterpret_cast<const void *>(&(fn))))

/* Maintains original function. */
/* Starts at function entry. Preserves arguments. */
#define INSTALL_IMPL_HOOK_ORIG(site, fn, out_orig) \
    (::ams::ldr::hoc::pcv::Hooks().InstallImpl((site), reinterpret_cast<const void *>(&(fn)), (out_orig)))

/* Takes the same arguments as the hooked function, does not replace it and cannot change what it does */
/* but it can be placed anywhere, not just at a function entry. */
#define INSTALL_INTERC_HOOK(site, fn) \
    (::ams::ldr::hoc::pcv::Hooks().InstallIntercept((site), reinterpret_cast<const void *>(&(fn))))
