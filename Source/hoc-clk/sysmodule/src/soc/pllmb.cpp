/*
 * Copyright (c) Souldbminer
 *
 * Copyright (c) KazushiMe
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

#include "pllmb.hpp"

namespace pllmb {
#define GET_BITS(VAL, HIGH, LOW) ((VAL & ((1UL << (HIGH + 1UL)) - 1UL)) >> LOW)
#define GET_BIT(VAL, BIT) GET_BITS(VAL, BIT, BIT)

    static inline volatile u32 &REG(uintptr_t addr) {
        return *reinterpret_cast<volatile u32 *>(addr);
    }

    // From jetson nano kernel
    typedef enum {
        /* divider = 2 */
        CLK_PLLX = 5,
        CLK_PLLM = 2,
        CLK_PLLMB = 37,
        /* PLLX & PLLG are backup PLLs for CPU & GPU */
        /* divider = 1 */
        CLK_CCLK_G = 18,  // A57 CPU cluster
        CLK_EMC = 36,
    } PTO_ID;  // PLL Test Output Register ID

    /* See if GM20B clock GPC PLL regs are accessible. */

#define PLLX_MISC0 0xE4
#define PLLM_MISC2 0x9C

    double ptoGetMHz(PTO_ID pto_id, u32 divider = 1, u32 presel_reg = 0, u32 presel_mask = 0) {
        u32 pre_val, val, presel_val;

        if (presel_reg) {
            val = REG(board::clkVirtAddr + presel_reg);
            usleep(10);
            presel_val = val & presel_mask;
            val &= ~presel_mask;
            val |= presel_mask;
            REG(board::clkVirtAddr + presel_reg) = val;
            usleep(10);
        }

        constexpr u32 cycle_count = 16;
        pre_val = REG(board::clkVirtAddr + 0x60);
        val = BIT(23) | BIT(13) | (cycle_count - 1);
        val |= pto_id << 14;

        REG(board::clkVirtAddr + 0x60) = val;
        usleep(10);
        REG(board::clkVirtAddr + 0x60) = val | BIT(10);
        usleep(10);
        REG(board::clkVirtAddr + 0x60) = val;
        usleep(10);
        REG(board::clkVirtAddr + 0x60) = val | BIT(9);
        usleep(500);

        while (REG(board::clkVirtAddr + 0x64) & BIT(31))
            ;

        val = REG(board::clkVirtAddr + 0x64);
        val &= 0xFFFFFF;
        val *= divider;

        double rate_hz = (u64)val * 32768. / cycle_count;
        usleep(10);
        REG(board::clkVirtAddr + 0x60) = pre_val;
        usleep(10);

        if (presel_reg) {
            val = REG(board::clkVirtAddr + presel_reg);
            usleep(10);
            val &= ~presel_mask;
            val |= presel_val;
            REG(board::clkVirtAddr + presel_reg) = val;
            usleep(10);
        }

        return rate_hz;
    }

    u64 getRamClockRatePLLMB() {
        // printf("\n"
        //        "EMC:    %6.1f MHz\n"
        //        "CCLK_G: %6.1f MHz\n"
        //        "PLLX:   %6.1f MHz\n"
        //        "PLLM:   %6.1f MHz\n"
        //        "PLLMB:  %6.1f MHz\n",
        //        ptoGetMHz(CLK_EMC),
        //        ptoGetMHz(CLK_CCLK_G),
        //        ptoGetMHz(CLK_PLLX,  2, PLLX_MISC0, BIT(22)),
        //        ptoGetMHz(CLK_PLLM,  2, PLLM_MISC2, BIT(8)),
        //        ptoGetMHz(CLK_PLLMB, 2, PLLM_MISC2, BIT(9))
        //        );

        u32 pllmb = ptoGetMHz(CLK_PLLMB, 2, PLLM_MISC2, BIT(9));
        u32 pllm = ptoGetMHz(CLK_PLLM, 2, PLLM_MISC2, BIT(8));
        return pllmb == 0 ? pllm : pllmb;  // pllmb is zeroed out at times, fallback to pllm
    }
}  // namespace pllmb