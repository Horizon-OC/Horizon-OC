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

#include "nvsrv.hpp"

namespace ams::ldr::hoc::nvsrv {

    namespace {

        /* NvServices device names */
        constexpr char DbgGpuPath[]  = "/dev/nvhost-dbg-gpu";
        constexpr char ProfGpuPath[] = "/dev/nvhost-prof-gpu";
        
        /* Permission offset to patch */
        constexpr size_t DevTablePermOffset = 0x68;

        uintptr_t FindBytes(uintptr_t start, uintptr_t end, const void *needle, size_t len) {
            const u8 *pat = reinterpret_cast<const u8 *>(needle);
            for (uintptr_t addr = start; addr + len <= end; ++addr) {
                if (std::memcmp(reinterpret_cast<const void *>(addr), pat, len) == 0) {
                    return addr;
                }
            }
            return 0;
        }

        /* Unlock the devices */
        Result UnlockDevicePermission(uintptr_t mapped_nso, size_t nso_size, uintptr_t nso_address, const char *path, const char *description) {
            const size_t path_len = std::strlen(path) + 1;

            const uintptr_t str_addr = FindBytes(mapped_nso, mapped_nso + nso_size, path, path_len);
            if (str_addr == 0) {
                LOGGING("nvsrv: %s path string not found (nso_size=%lx)", description, static_cast<unsigned long>(nso_size));
                R_THROW(ldr::ResultInvalidNvsrvDevTable());
            }

            const uintptr_t str_offset  = str_addr - mapped_nso;
            const uintptr_t candidates[] = { str_offset, nso_address + str_offset };

            uintptr_t entry_addr = 0;
            for (uintptr_t ptr = mapped_nso; entry_addr == 0 && ptr + sizeof(uintptr_t) <= mapped_nso + nso_size; ptr += sizeof(uintptr_t)) {
                const uintptr_t val = *reinterpret_cast<const uintptr_t *>(ptr);
                for (uintptr_t cand : candidates) {
                    if (val == cand) {
                        entry_addr = ptr;
                        break;
                    }
                }
            }
            if (entry_addr == 0) {
                LOGGING("nvsrv: %s str@+%lx has no referencing table entry", description,
                        static_cast<unsigned long>(str_offset), static_cast<unsigned long>(candidates[0]), static_cast<unsigned long>(candidates[1]));
                R_THROW(ldr::ResultInvalidNvsrvDevTable());
            }

            const uintptr_t perm_addr = entry_addr + DevTablePermOffset;
            if (perm_addr + sizeof(uintptr_t) > mapped_nso + nso_size) {
                LOGGING("nvsrv: %s entry@+%lx perm field out of bounds", description, static_cast<unsigned long>(entry_addr - mapped_nso));
                R_THROW(ldr::ResultInvalidNvsrvDevTable());
            }

            uintptr_t *perm = reinterpret_cast<uintptr_t *>(perm_addr);
            LOGGING("nvsrv: %s entry@+%lx perm@+%lx was %lx", description,
                    static_cast<unsigned long>(entry_addr - mapped_nso),
                    static_cast<unsigned long>(perm_addr - mapped_nso),
                    static_cast<unsigned long>(*perm));

            PATCH_OFFSET(perm, static_cast<uintptr_t>(0));
            R_SUCCEED();
        }

    }

    void Patch(uintptr_t mapped_nso, size_t nso_size, uintptr_t nso_address) {
        struct {
            const char *path;
            const char *description;
        } targets[] = {
            { DbgGpuPath,  "dbg-gpu"  },
            { ProfGpuPath, "prof-gpu" },
        };

        for (auto &t : targets) {
            const Result res = UnlockDevicePermission(mapped_nso, nso_size, nso_address, t.path, t.description);
            LOGGING("nvsrv: %s unlock%s", t.description, R_SUCCEEDED(res) ? "ed" : " skipped");
        }
    }

}
