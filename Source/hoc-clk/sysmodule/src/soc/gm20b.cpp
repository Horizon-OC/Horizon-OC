/*
 * Copyright (c) NVIDIA
 *
 * Copyright (c) Souldbminer
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

#include "../mapping/mem_map.hpp"
#include "gm20b.hpp"


namespace gm20b {
    u64 gpu_base = 0;
#define GPU_PA 0x57000000
#define GPU_SIZE 0x1000000
#define GPU_TRIM_SYS_GPCPLL_CFG 0x00
#define GPU_TRIM_SYS_GPCPLL_COEFF 0x04
#define GPU_TRIM_SYS_GPCPLL_CFG2 0x0C
#define GPU_TRIM_SYS_GPCPLL_CFG3 0x18
#define GPU_TRIM_SYS_GPCPLL_DVFS0 0x10
#define GPU_TRIM_SYS_GPCPLL_DVFS1 0x14
#define GPU_TRIM_SYS_GPCPLL_NDIV_SLOWDOWN 0x1C
#define GPU_TRIM_SYS_SEL_VCO 0x100
#define GPU_TRIM_SYS_GPC2CLK_OUT 0x250

#define GPC_BCAST(x) (*(volatile u32 *)(gpu_base + 0x132800ul + (x)))
#define GPU_TRIM_SYS_GPCPLL(x) (*(volatile u32 *)(gpu_base + 0x137000ul + (x)))

#define GPC_BCAST_GPCPLL_DVFS2 0x20
#define GPC_BCAST_NDIV_SLOWDOWN_DBG 0xa0

#define GPCPLL_CFG_ENABLE BIT(0)
#define GPCPLL_CFG_IDDQ BIT(1)
#define GPCPLL_CFG_SYNC_MODE BIT(2)
#define GPCPLL_CFG_LOCK BIT(17)

#define GPCPLL_CFG2_SDM_DIN_MASK 0x000000FFu
#define GPCPLL_CFG2_SDM_DIN_NEW_MASK 0x0000FF00u
#define GPCPLL_CFG2_STEPA_SHIFT 24
#define GPCPLL_CFG2_STEPA_MASK 0xFF000000u

#define GPCPLL_CFG3_STEPB_SHIFT 16
#define GPCPLL_CFG3_STEPB_MASK 0x00FF0000u

#define GPCPLL_DVFS0_DFS_COEFF_MASK 0x0000007Fu

#define NDIV_SLOWDOWN_SLOWDOWN_USING_PLL BIT(22)
#define NDIV_SLOWDOWN_EN_DYNRAMP BIT(31)

#define DYNRAMP_DONE_SYNCED BIT(24)

#define GPCPLL_DVFS2_DFS_EXT_STROBE BIT(16)

#define RAMP_TIMEOUT_US 500

    static inline void _gpu_mask(u32 reg, u32 mask, u32 val) {
        u32 tmp = GPU_TRIM_SYS_GPCPLL(reg);
        GPU_TRIM_SYS_GPCPLL(reg) = (tmp & ~mask) | (val & mask);
        (void)GPU_TRIM_SYS_GPCPLL(reg);
    }

    static inline void _gbc_mask(u32 reg, u32 mask, u32 val) {
        u32 tmp = GPC_BCAST(reg);
        GPC_BCAST(reg) = (tmp & ~mask) | (val & mask);
        (void)GPC_BCAST(reg);
    }

    static void _clk_setup_slide() {
        _gpu_mask(GPU_TRIM_SYS_GPCPLL_CFG2, GPCPLL_CFG2_STEPA_MASK, 0x04u << GPCPLL_CFG2_STEPA_SHIFT);
        _gpu_mask(GPU_TRIM_SYS_GPCPLL_CFG3, GPCPLL_CFG3_STEPB_MASK, 0x05u << GPCPLL_CFG3_STEPB_SHIFT);
    }

    static bool _gpu_pllg_slide(u32 new_divn) {
        u32 coeff = GPU_TRIM_SYS_GPCPLL(GPU_TRIM_SYS_GPCPLL_COEFF);
        u32 cur_divn = (coeff >> 8) & 0xFF;

        if (new_divn == cur_divn)
            return true;

        _clk_setup_slide();

        _gpu_mask(GPU_TRIM_SYS_GPCPLL_NDIV_SLOWDOWN, NDIV_SLOWDOWN_SLOWDOWN_USING_PLL, NDIV_SLOWDOWN_SLOWDOWN_USING_PLL);

        _gpu_mask(GPU_TRIM_SYS_GPCPLL_CFG2, GPCPLL_CFG2_SDM_DIN_NEW_MASK, 0);

        coeff = (coeff & ~(0xFFu << 8)) | (new_divn << 8);
        usleep(1);
        GPU_TRIM_SYS_GPCPLL(GPU_TRIM_SYS_GPCPLL_COEFF) = coeff;

        usleep(1);
        _gpu_mask(GPU_TRIM_SYS_GPCPLL_NDIV_SLOWDOWN, NDIV_SLOWDOWN_EN_DYNRAMP, NDIV_SLOWDOWN_EN_DYNRAMP);

        int ramp_timeout = RAMP_TIMEOUT_US;
        bool success = false;
        do {
            usleep(1);
            ramp_timeout--;
            if (GPC_BCAST(GPC_BCAST_NDIV_SLOWDOWN_DBG) & DYNRAMP_DONE_SYNCED) {
                success = true;
                break;
            }
        } while (ramp_timeout > 0);

        if (success) {
            _gpu_mask(GPU_TRIM_SYS_GPCPLL_CFG2, GPCPLL_CFG2_SDM_DIN_MASK, 0);
        }

        _gpu_mask(GPU_TRIM_SYS_GPCPLL_NDIV_SLOWDOWN, NDIV_SLOWDOWN_SLOWDOWN_USING_PLL | NDIV_SLOWDOWN_EN_DYNRAMP, 0);
        (void)GPU_TRIM_SYS_GPCPLL(GPU_TRIM_SYS_GPCPLL_NDIV_SLOWDOWN);

        return success;
    }

    bool setClock(u32 khz) {
        if (!gpu_base)
            QueryMemoryMapping(&gpu_base, GPU_PA, GPU_SIZE);

        const u32 osc_khz = 38400;  // PLL Hz

        u32 coeff = GPU_TRIM_SYS_GPCPLL(GPU_TRIM_SYS_GPCPLL_COEFF);
        u32 divm = coeff & 0xFF;
        u32 divp = (coeff >> 16) & 0x3F;

        if (divm == 0 || divp == 0)
            return false;

        u32 new_divn = (u64)khz * divm * divp * 2 / osc_khz;

        // L4T clamps the registers here for some reason, do that
        if (new_divn < 8)
            new_divn = 8;
        if (new_divn > 255)
            new_divn = 255;

        return _gpu_pllg_slide(new_divn);
    }
}  // namespace gm20b