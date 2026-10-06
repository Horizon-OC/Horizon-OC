/*
 * Copyright (c) Souldbminer, based on reasearch by MasaGratoR and Cooler3D
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

#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <switch.h>

#include "display_refresh_rate.hpp"

namespace display {
#define DSI_CLOCK_HZ 234000000llu
#define NVDISP_GET_MODE2 0x803C021B
#define NVDISP_SET_MODE2 0x403C021C
#define NVDISP_VALIDATE_MODE2 0xC03C021D
#define NVDISP_GET_MODE_DB2 0xEF20021E
#define NVDISP_GET_PANEL_DATA 0xC01C0226

#define MAX_REFRESH_RATE 72

    static DisplayRefreshConfig g_config = { 0 };
    static bool g_initialized = false;

    static uint8_t g_dockedHighestRefreshRate = 60;
    static uint8_t g_dockedLinkRate = 10;
    static bool g_wasRetroSuperTurnedOff = false;
    static uint32_t g_lastVActive = 1080;
    static bool g_canChangeRefreshRateDocked = false;
    static uint8_t g_lastVActiveSet = 0;

    static const uint8_t g_dockedRefreshRates[] = { 40,  45,  50,  55,  60,  70,  72,  75,  80,  90,  95,  100, 110, 120,
                                                    130, 140, 144, 150, 160, 165, 170, 180, 190, 200, 210, 220, 230, 240 };
    // Calculate with this tool:

    // https://tomverbeure.github.io/video_timings_calculator?horiz_pixels=1920&vert_pixels=1080&refresh_rate=240&margins=false&interlaced=false&bpc=8&color_fmt=rgb444&video_opt=false&custom_hblank=80&custom_vblank=6

    /*
    typedef struct {
        uint16_t hFrontPorch;
        uint8_t hSyncWidth;
        uint8_t hBackPorch;
        uint8_t vFrontPorch;
        uint8_t vSyncWidth;
        uint8_t vBackPorch;
        uint8_t VIC;
        uint32_t pixelClock_kHz;
    } DockedTimings;
    */
    static const DockedTimings g_dockedTimings1080p[] = {
        { 8, 32, 40, 7, 8, 6, 0, 88080 },        // 40Hz
        { 8, 32, 40, 9, 8, 6, 0, 99270 },        // 45Hz
        { 528, 44, 148, 4, 5, 36, 31, 148500 },  // 50Hz
        { 8, 32, 40, 15, 8, 6, 0, 121990 },      // 55Hz
        { 88, 44, 148, 4, 5, 36, 16, 148500 },   // 60Hz
        { 8, 32, 40, 22, 8, 6, 0, 156240 },      // 70Hz
        { 8, 32, 40, 23, 8, 6, 0, 160848 },      // 72Hz
        { 8, 32, 40, 25, 8, 6, 0, 167850 },      // 75Hz
        { 8, 32, 40, 28, 8, 6, 0, 179520 },      // 80Hz
        { 8, 32, 40, 33, 8, 6, 0, 202860 },      // 90Hz
        { 8, 32, 40, 36, 8, 6, 0, 214700 },      // 95Hz
        { 528, 44, 148, 4, 5, 36, 64, 297000 },  // 100Hz
        { 8, 32, 40, 44, 8, 6, 0, 250360 },      // 110Hz
        { 88, 44, 148, 4, 5, 36, 63, 297000 },   // 120Hz
        { 8, 32, 40, 55, 8, 6, 0, 298750 },      // 130Hz CVT-RBv2
        { 8, 32, 40, 61, 8, 6, 0, 323400 },      // 140Hz CVT-RBv2
        { 8, 32, 40, 63, 8, 6, 0, 333216 },      // 144Hz CVT-RBv2
        { 8, 32, 40, 67, 8, 6, 0, 348300 },      // 150Hz CVT-RBv2
        { 8, 32, 40, 72, 8, 6, 0, 373120 },      // 160Hz CVT-RBv2
        { 8, 32, 40, 75, 8, 6, 0, 385770 },      // 165Hz CVT-RBv2
        { 8, 32, 40, 78, 8, 6, 0, 398480 },      // 170Hz CVT-RBv2
        { 8, 32, 40, 84, 8, 6, 0, 424080 },      // 180Hz CVT-RBv2
        { 8, 32, 40, 90, 8, 6, 0, 449920 },      // 190Hz CVT-RBv2
        { 8, 32, 40, 96, 8, 6, 0, 476000 },      // 200Hz CVT-RBv2
        { 8, 32, 40, 102, 8, 6, 0, 502320 },     // 210Hz CVT-RBv2
        { 8, 32, 40, 108, 8, 6, 0, 528880 },     // 220Hz CVT-RBv2
        { 8, 32, 40, 114, 8, 6, 0, 555680 },     // 230Hz CVT-RBv2
        { 8, 32, 40, 121, 8, 6, 0, 583200 },     // 240Hz CVT-RBv2
        // technically you can go to 476hz, but in practice, why would you?
    };

    // These timings *should* work but are untested
    static const HandheldTimings g_handheldTimingsRETRO[] = {
        { 72, 136, 72, 1, 660, 9, 78000 },  // 40Hz
        { 72, 136, 72, 1, 612, 9, 77982 },  // 41Hz
        { 72, 136, 72, 1, 567, 9, 77994 },  // 42Hz
        { 72, 136, 72, 1, 524, 9, 78002 },  // 43Hz
        { 72, 136, 72, 1, 483, 9, 78012 },  // 44Hz
        { 72, 136, 72, 1, 443, 9, 77985 },  // 45Hz
        { 72, 136, 72, 1, 406, 9, 78016 },  // 46Hz
        { 72, 136, 72, 1, 370, 9, 78020 },  // 47Hz
        { 72, 136, 72, 1, 335, 9, 78000 },  // 48Hz
        { 72, 136, 72, 1, 302, 9, 78008 },  // 49Hz
        { 72, 136, 72, 1, 270, 9, 78000 },  // 50Hz
        { 72, 136, 72, 1, 239, 9, 77979 },  // 51Hz
        { 72, 136, 72, 1, 210, 9, 78000 },  // 52Hz
        { 72, 136, 72, 1, 182, 9, 78016 },  // 53Hz
        { 72, 136, 72, 1, 154, 9, 77976 },  // 54Hz
        { 72, 136, 72, 1, 128, 9, 77990 },  // 55Hz
        { 72, 136, 72, 1, 103, 9, 78008 },  // 56Hz
        { 72, 136, 72, 1, 78, 9, 77976 },   // 57Hz
        { 72, 136, 72, 1, 55, 9, 78010 },   // 58Hz
        { 72, 136, 72, 1, 32, 9, 77998 },   // 59Hz
        { 72, 136, 72, 1, 10, 9, 78000 },   // 60Hz
    };

    static const MinMaxRefreshRate g_handheldModeRefreshRate = { 40, 80 };

    static uint8_t _getDockedRefreshRateIterator(uint32_t refreshRate) {
        for (size_t i = 0; i < sizeof(g_dockedRefreshRates) / sizeof(g_dockedRefreshRates[0]); i++) {
            if (g_dockedRefreshRates[i] == refreshRate)
                return i;
        }
        return 0xFF;
    }

    static void _changeOledElvssSettings(const uint32_t *offsets, const uint32_t *value, uint32_t size, uint32_t start) {
        if (!g_config.dsiVirtAddr || !value || !size)
            return;

        volatile uint32_t *dsi = (uint32_t *)g_config.dsiVirtAddr;

#define DSI_VIDEO_MODE_CONTROL 0x4E
#define DSI_WR_DATA 0xA
#define DSI_TRIGGER 0x13
#define MIPI_DSI_DCS_SHORT_WRITE_PARAM 0x15
#define MIPI_DSI_DCS_LONG_WRITE 0x39
#define MIPI_DCS_PRIV_SM_SET_REG_OFFSET 0xB0
#define MIPI_DCS_PRIV_SM_SET_ELVSS 0xB1

        dsi[DSI_VIDEO_MODE_CONTROL] = true;
        svcSleepThread(20000000);

        dsi[DSI_WR_DATA] = MIPI_DSI_DCS_LONG_WRITE | (5 << 8);
        dsi[DSI_WR_DATA] = 0x5A5A5AE2;
        dsi[DSI_WR_DATA] = 0x5A;
        dsi[DSI_TRIGGER] = 0;

        for (size_t i = start; i < size; i++) {
            dsi[DSI_WR_DATA] = ((MIPI_DCS_PRIV_SM_SET_REG_OFFSET | ((offsets[i] % 0x100) << 8)) << 8) | MIPI_DSI_DCS_SHORT_WRITE_PARAM;
            dsi[DSI_TRIGGER] = 0;
            dsi[DSI_WR_DATA] = ((MIPI_DCS_PRIV_SM_SET_ELVSS | (value[i] << 8)) << 8) | MIPI_DSI_DCS_SHORT_WRITE_PARAM;
            dsi[DSI_TRIGGER] = 0;
        }

        dsi[DSI_WR_DATA] = MIPI_DSI_DCS_LONG_WRITE | (5 << 8);
        dsi[DSI_WR_DATA] = 0xA55A5AE2;
        dsi[DSI_WR_DATA] = 0xA5;
        dsi[DSI_TRIGGER] = 0;

        dsi[DSI_VIDEO_MODE_CONTROL] = false;
        svcSleepThread(20000000);
    }
    void SetDockedState(bool isDocked) {
        g_config.isDocked = isDocked;
    }

    bool Initialize(const DisplayRefreshConfig *config) {
        if (!config)
            return false;

        g_config = *config;
        g_initialized = true;
        return true;
    }

    void CorrectOledGamma(uint32_t refresh_rate) {
        static uint32_t last_refresh_rate = 60;
        static int counter = 0;

        if (g_config.isDocked || refresh_rate < 45 || refresh_rate > 60) {
            last_refresh_rate = 60;
            return;
        }

        if (counter != 9) {
            counter++;
            return;
        }
        counter = 0;

        uint32_t offsets[] = { 0x1A, 0x24, 0x25 };
        uint32_t values[] = { 2, 0, 0x83 };

        if (refresh_rate == 60) {
            if (last_refresh_rate == 60)
                return;
        } else if (refresh_rate == 45) {
            if (last_refresh_rate == 45)
                return;
            uint32_t vals[] = { 4, 1, 0 };
            memcpy(values, vals, sizeof(vals));
        } else if (refresh_rate == 50) {
            if (last_refresh_rate == 50)
                return;
            uint32_t vals[] = { 3, 1, 0 };
            memcpy(values, vals, sizeof(vals));
        } else if (refresh_rate == 55) {
            if (last_refresh_rate == 55)
                return;
            uint32_t vals[] = { 3, 1, 0 };
            memcpy(values, vals, sizeof(vals));
        } else {
            return;
        }

        for (int i = 0; i < 5; i++) {
            _changeOledElvssSettings(offsets, values, 3, 0);
        }
        last_refresh_rate = refresh_rate;
    }

    void SetAllowedDockedRatesIPC(uint32_t refreshRates, bool is720p) {
        // Function kept for API compatibility but does nothing
        (void)refreshRates;
        (void)is720p;
    }

    uint8_t GetDockedHighestAllowed(void) {
        return (g_dockedHighestRefreshRate > 60) ? g_dockedHighestRefreshRate : 60;
    }

    static void _getDockedHighestRefreshRate(uint32_t fd_in) {
        uint8_t highestRefreshRate = 60;
        uint32_t fd = fd_in;

        if (!fd)
            nvOpen(&fd, "/dev/nvdisp-disp1");
        NvdcModeDB2 db2 = { 0 };
        int rc = nvIoctl(fd, NVDISP_GET_MODE_DB2, &db2);

        if (rc == 0) {
            for (size_t i = 0; i < db2.num_modes; i++) {
                if (db2.modes[i].hActive < 1920 || db2.modes[i].vActive < 1080)
                    continue;

                uint32_t v_total = db2.modes[i].vActive + db2.modes[i].vSyncWidth + db2.modes[i].vFrontPorch + db2.modes[i].vBackPorch;
                uint32_t h_total = db2.modes[i].hActive + db2.modes[i].hSyncWidth + db2.modes[i].hFrontPorch + db2.modes[i].hBackPorch;
                double refreshRate = round((double)(db2.modes[i].pclkKHz * 1000) / (double)(v_total * h_total));

                if (highestRefreshRate < (uint8_t)refreshRate)
                    highestRefreshRate = (uint8_t)refreshRate;
            }
        } else {
            g_dockedHighestRefreshRate = 60;
        }

        const size_t numRates = sizeof(g_dockedRefreshRates) / sizeof(g_dockedRefreshRates[0]);
        if (highestRefreshRate > g_dockedRefreshRates[numRates - 1])
            highestRefreshRate = g_dockedRefreshRates[numRates - 1];

        NvdcMode2 display_b = { 0 };
        rc = nvIoctl(fd, NVDISP_GET_MODE2, &display_b);

        struct dpaux_read_0x100 {
            uint32_t cmd;
            uint32_t addr;
            uint32_t size;
            struct {
                unsigned char link_rate;
                unsigned int lane_count : 5;
                unsigned int unk1 : 2;
                unsigned int isFramingEnhanced : 1;
                unsigned char downspread;
                unsigned char training_pattern;
                unsigned char lane_pattern[4];
                unsigned char unk2[8];
            } set;
        } dpaux = { 6, 0x100, 0x10 };

        rc = nvIoctl(fd, NVDISP_GET_PANEL_DATA, &dpaux);
        if (rc == 0) {
            g_dockedLinkRate = dpaux.set.link_rate;
            // if (display_b.hActive == 1920 && display_b.vActive == 1080 && highestRefreshRate > 75 && dpaux.set.link_rate < 20 && )
            //     highestRefreshRate = 75;
        }

        if (!fd_in)
            nvClose(fd);
        g_dockedHighestRefreshRate = highestRefreshRate;
    }

    static bool _setPLLDHandheldRefreshRate(uint32_t new_refreshRate) {
        if (!g_config.clkVirtAddr)
            return false;

        uint32_t fd = 0;
        if (nvOpen(&fd, "/dev/nvdisp-disp0")) {
            return false;
        }

        struct dpaux_read {
            uint32_t cmd;
            uint32_t addr;
            uint32_t size;
            struct {
                unsigned int rev_minor : 4;
                unsigned int rev_major : 4;
                unsigned char link_rate;
                unsigned int lane_count : 5;
                unsigned int unk1 : 2;
                unsigned int isFramingEnhanced : 1;
                unsigned char unk2[13];
            } DPCD;
        } dpaux = { 6, 0, 0x10 };

        int rc = nvIoctl(fd, NVDISP_GET_PANEL_DATA, &dpaux);
        nvClose(fd);
        if (rc != 0x75c)
            return false;

        PLLD_BASE base = { 0 };
        PLLD_MISC misc = { 0 };
        memcpy(&base, (void *)(g_config.clkVirtAddr + 0xD0), 4);
        memcpy(&misc, (void *)(g_config.clkVirtAddr + 0xDC), 4);

        uint32_t value = ((base.PLLD_DIVN / base.PLLD_DIVM) * 10) / 4;
        if (value == 0 || value == 80)
            return false;

        if (new_refreshRate > g_handheldModeRefreshRate.max) {
            new_refreshRate = g_handheldModeRefreshRate.max;
        } else if (new_refreshRate < g_handheldModeRefreshRate.min) {
            bool skip = false;
            for (size_t i = 2; i <= 4; i++) {
                if (new_refreshRate * i == 60) {
                    skip = true;
                    new_refreshRate = 60;
                    break;
                }
            }
            if (!skip) {
                for (size_t i = 2; i <= 4; i++) {
                    if (((new_refreshRate * i) >= g_handheldModeRefreshRate.min) && ((new_refreshRate * i) <= g_handheldModeRefreshRate.max)) {
                        skip = true;
                        new_refreshRate *= i;
                        break;
                    }
                }
            }
            if (!skip)
                new_refreshRate = 60;
        }

        uint32_t pixelClock = (9375 * ((4096 * ((2 * base.PLLD_DIVN) + 1)) + misc.PLLD_SDM_DIN)) / (8 * base.PLLD_DIVM);
        uint16_t refreshRateNow = pixelClock / (DSI_CLOCK_HZ / 60);

        if (refreshRateNow == new_refreshRate) {
            return true;
        }

        uint8_t base_refreshRate = new_refreshRate - (new_refreshRate % 5);
        base.PLLD_DIVN = (4 * base_refreshRate) / 10;
        base.PLLD_DIVM = 1;

        uint64_t expected_pixel_clock = (DSI_CLOCK_HZ * new_refreshRate) / 60;
        misc.PLLD_SDM_DIN = ((8 * base.PLLD_DIVM * expected_pixel_clock) / 9375) - (4096 * ((2 * base.PLLD_DIVN) + 1));

        memcpy((void *)(g_config.clkVirtAddr + 0xD0), &base, 4);
        memcpy((void *)(g_config.clkVirtAddr + 0xDC), &misc, 4);
        return true;
    }

    static bool _setNvDispDockedRefreshRate(uint32_t new_refreshRate) {
        if (g_config.isLite || !g_canChangeRefreshRateDocked)
            return false;

        uint32_t fd = 0;
        if (nvOpen(&fd, "/dev/nvdisp-disp1")) {
            return false;
        }

        NvdcMode2 display_b = { 0 };
        int rc = nvIoctl(fd, NVDISP_GET_MODE2, &display_b);
        if (rc != 0) {
            nvClose(fd);
            return false;
        }

        if (!display_b.pclkKHz) {
            nvClose(fd);
            return false;
        }

        if (!((display_b.vActive == 480 && display_b.hActive == 720) || (display_b.vActive == 720 && display_b.hActive == 1280) ||
              (display_b.vActive == 1080 && display_b.hActive == 1920))) {
            nvClose(fd);
            return false;
        }

        if (display_b.vActive != g_lastVActiveSet) {
            g_lastVActiveSet = display_b.vActive;
        }

        uint32_t h_total = display_b.hActive + display_b.hFrontPorch + display_b.hSyncWidth + display_b.hBackPorch;
        uint32_t v_total = display_b.vActive + display_b.vFrontPorch + display_b.vSyncWidth + display_b.vBackPorch;
        uint32_t refreshRateNow = ((display_b.pclkKHz) * 1000 + 999) / (h_total * v_total);

        int8_t itr = -1;
        const size_t numRates = sizeof(g_dockedRefreshRates) / sizeof(g_dockedRefreshRates[0]);

        // Find closest matching refresh rate
        if ((new_refreshRate <= 60) && ((60 % new_refreshRate) == 0)) {
            itr = _getDockedRefreshRateIterator(60);
        }

        if (itr == -1) {
            for (size_t i = 0; i < numRates; i++) {
                uint8_t val = g_dockedRefreshRates[i];
                if ((val % new_refreshRate) == 0) {
                    itr = i;
                    break;
                }
            }
        }

        if (itr == -1) {
            if (!g_config.matchLowestDocked) {
                itr = _getDockedRefreshRateIterator(60);
            } else {
                for (size_t i = 0; i < numRates; i++) {
                    if (new_refreshRate < g_dockedRefreshRates[i]) {
                        itr = i;
                        break;
                    }
                }
            }
        }

        if (itr == -1)
            itr = _getDockedRefreshRateIterator(60);

        // Clamp to highest allowed refresh rate
        if (g_dockedRefreshRates[itr] > g_dockedHighestRefreshRate) {
            for (int8_t i = itr; i >= 0; i--) {
                if (g_dockedRefreshRates[i] <= g_dockedHighestRefreshRate) {
                    itr = i;
                    break;
                }
            }
        }

        if (refreshRateNow == g_dockedRefreshRates[itr]) {
            nvClose(fd);
            return true;
        }

        if (itr >= 0 && itr < (int8_t)numRates) {
            if (display_b.vActive == 720) {
                uint32_t clock = ((h_total * v_total) * g_dockedRefreshRates[itr]) / 1000;
                display_b.pclkKHz = clock;
            } else {
                display_b.hFrontPorch = g_dockedTimings1080p[itr].hFrontPorch;
                display_b.hSyncWidth = g_dockedTimings1080p[itr].hSyncWidth;
                display_b.hBackPorch = g_dockedTimings1080p[itr].hBackPorch;
                display_b.vFrontPorch = g_dockedTimings1080p[itr].vFrontPorch;
                display_b.vSyncWidth = g_dockedTimings1080p[itr].vSyncWidth;
                display_b.vBackPorch = g_dockedTimings1080p[itr].vBackPorch;
                display_b.pclkKHz = g_dockedTimings1080p[itr].pixelClock_kHz;
                display_b.vmode = (g_dockedRefreshRates[itr] >= 100 ? 0x400000 : 0x200000);
                display_b.unk1 = (g_dockedRefreshRates[itr] >= 100 ? 0x80 : 0);
                display_b.sync = 3;
                display_b.bitsPerPixel = 24;
            }

            rc = nvIoctl(fd, NVDISP_VALIDATE_MODE2, &display_b);
            if (rc == 0) {
                rc = nvIoctl(fd, NVDISP_SET_MODE2, &display_b);
            }
        }

        nvClose(fd);
        return true;
    }

    static bool _setNvDispHandheldRefreshRate(uint32_t new_refreshRate) {
        if (!g_config.isRetroSUPER)
            return false;

        if (!g_config.displaySync) {
            g_wasRetroSuperTurnedOff = false;
        } else if (g_wasRetroSuperTurnedOff) {
            svcSleepThread(2000000000);
            g_wasRetroSuperTurnedOff = false;
        }

        svcSleepThread(1000000000);

        uint32_t fd = 0;
        if (nvOpen(&fd, "/dev/nvdisp-disp0")) {
            return false;
        }

        NvdcMode2 display_b = { 0 };
        int rc = nvIoctl(fd, NVDISP_GET_MODE2, &display_b);
        if (rc != 0) {
            nvClose(fd);
            return false;
        }

        if (!display_b.pclkKHz) {
            nvClose(fd);
            return false;
        }

        if ((display_b.vActive == 1280 && display_b.hActive == 720) == false) {
            nvClose(fd);
            return false;
        }

        uint32_t h_total = display_b.hActive + display_b.hFrontPorch + display_b.hSyncWidth + display_b.hBackPorch;
        uint32_t v_total = display_b.vActive + display_b.vFrontPorch + display_b.vSyncWidth + display_b.vBackPorch;
        uint32_t refreshRateNow = ((display_b.pclkKHz) * 1000 + 999) / (h_total * v_total);

        if (new_refreshRate > g_handheldModeRefreshRate.max) {
            new_refreshRate = g_handheldModeRefreshRate.max;
        } else if (new_refreshRate < g_handheldModeRefreshRate.min) {
            bool skip = false;
            for (size_t i = 2; i <= 4; i++) {
                if (new_refreshRate * i == 60) {
                    skip = true;
                    new_refreshRate = 60;
                    break;
                }
            }
            if (!skip) {
                for (size_t i = 2; i <= (sizeof(g_handheldTimingsRETRO) / sizeof(g_handheldTimingsRETRO[0])); i++) {
                    if (((new_refreshRate * i) >= g_handheldModeRefreshRate.min) && ((new_refreshRate * i) <= g_handheldModeRefreshRate.max)) {
                        skip = true;
                        new_refreshRate *= i;
                        break;
                    }
                }
            }
            if (!skip)
                new_refreshRate = 60;
        }

        if (new_refreshRate == refreshRateNow) {
            nvClose(fd);
            return true;
        }

        uint32_t itr = (new_refreshRate - 40) / 5;
        display_b.hFrontPorch = g_handheldTimingsRETRO[itr].hFrontPorch;
        display_b.hSyncWidth = g_handheldTimingsRETRO[itr].hSyncWidth;
        display_b.hBackPorch = g_handheldTimingsRETRO[itr].hBackPorch;
        display_b.vFrontPorch = g_handheldTimingsRETRO[itr].vFrontPorch;
        display_b.vSyncWidth = g_handheldTimingsRETRO[itr].vSyncWidth;
        display_b.vBackPorch = g_handheldTimingsRETRO[itr].vBackPorch;
        display_b.pclkKHz = g_handheldTimingsRETRO[itr].pixelClock_kHz;

        rc = nvIoctl(fd, NVDISP_VALIDATE_MODE2, &display_b);
        if (rc == 0) {
            for (size_t i = 0; i < 5; i++) {
                nvIoctl(fd, NVDISP_SET_MODE2, &display_b);
            }
        }

        nvClose(fd);
        return true;
    }

    bool SetRate(uint32_t new_refreshRate) {
        if (!new_refreshRate || !g_initialized)
            return false;

        uint32_t fd = 0;

        if (g_config.isRetroSUPER && !g_config.isDocked) {
            return _setNvDispHandheldRefreshRate(new_refreshRate);
        }

        else if ((!g_config.isRetroSUPER && g_config.isLite) || R_FAILED(nvOpen(&fd, "/dev/nvdisp-disp1"))) {
            if (_setPLLDHandheldRefreshRate(new_refreshRate) == false)
                return false;
        } else {
            struct dpaux_read {
                uint32_t cmd;
                uint32_t addr;
                uint32_t size;
                struct {
                    unsigned int rev_minor : 4;
                    unsigned int rev_major : 4;
                    unsigned char link_rate;
                    unsigned int lane_count : 5;
                    unsigned int unk1 : 2;
                    unsigned int isFramingEnhanced : 1;
                    unsigned char unk2[13];
                } DPCD;
            } dpaux = { 6, 0, 0x10 };

            int rc = nvIoctl(fd, NVDISP_GET_PANEL_DATA, &dpaux);
            nvClose(fd);

            if (rc != 0) {
                if (!g_config.isRetroSUPER) {
                    return _setPLLDHandheldRefreshRate(new_refreshRate);
                } else {
                    return _setNvDispHandheldRefreshRate(new_refreshRate);
                }
            } else {
                if (g_config.isDocked)
                    return _setNvDispDockedRefreshRate(new_refreshRate);
                else
                    return true;
            }
        }
        return false;
    }

    bool GetRate(uint32_t *out_refreshRate, bool internal) {
        if (!out_refreshRate || !g_initialized || !g_config.clkVirtAddr)
            return false;
        static uint32_t value = 60;

        if (g_config.isRetroSUPER && !g_config.isDocked) {
            uint32_t fd = 0;
            PLLD_BASE temp = { 0 };
            PLLD_MISC misc = { 0 };
            memcpy(&temp, (void *)(g_config.clkVirtAddr + 0xD0), 4);
            memcpy(&misc, (void *)(g_config.clkVirtAddr + 0xDC), 4);

            value = ((temp.PLLD_DIVN / temp.PLLD_DIVM) * 10) / 4;

            if (value != 0 && value != 80) {
                if (!nvOpen(&fd, "/dev/nvdisp-disp0")) {
                    NvdcMode2 display_b = { 0 };
                    if (nvIoctl(fd, NVDISP_GET_MODE2, &display_b) == 0) {
                        uint64_t h_total = display_b.hActive + display_b.hFrontPorch + display_b.hSyncWidth + display_b.hBackPorch;
                        uint64_t v_total = display_b.vActive + display_b.vFrontPorch + display_b.vSyncWidth + display_b.vBackPorch;
                        uint64_t pixelClock = display_b.pclkKHz * 1000 + 999;
                        value = (u32)(pixelClock / (h_total * v_total));
                    }
                    nvClose(fd);
                } else {
                    return false;
                }
            } else {
                g_wasRetroSuperTurnedOff = true;
            }
        } else if ((!g_config.isPossiblySpoofedRetro) || (g_config.isPossiblySpoofedRetro && !g_config.isRetroSUPER)) {
            PLLD_BASE temp = { 0 };
            PLLD_MISC misc = { 0 };
            memcpy(&temp, (void *)(g_config.clkVirtAddr + 0xD0), 4);
            memcpy(&misc, (void *)(g_config.clkVirtAddr + 0xDC), 4);

            value = ((temp.PLLD_DIVN / temp.PLLD_DIVM) * 10) / 4;

            if (value == 0 || value == 80) {
                // Docked mode
                if (g_config.isLite)
                    return false;

                g_config.isDocked = true;

                if (!g_canChangeRefreshRateDocked) {
                    uint32_t fd = 0;
                    if (!nvOpen(&fd, "/dev/nvdisp-disp1")) {
                        struct dpaux_read_0x100 {
                            uint32_t cmd;
                            uint32_t addr;
                            uint32_t size;
                            struct {
                                unsigned char link_rate;
                                unsigned int lane_count : 5;
                                unsigned int unk1 : 2;
                                unsigned int isFramingEnhanced : 1;
                                unsigned char downspread;
                                unsigned char training_pattern;
                                unsigned char lane_pattern[4];
                                unsigned char unk2[8];
                            } set;
                        } dpaux = { 6, 0x100, 0x10 };

                        int rc = nvIoctl(fd, NVDISP_GET_PANEL_DATA, &dpaux);
                        nvClose(fd);

                        if (rc == 0) {
                            _getDockedHighestRefreshRate(0);
                            g_canChangeRefreshRateDocked = true;
                        } else {
                            svcSleepThread(1000000000);
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
                if (internal) {
                    *out_refreshRate = value;
                    return true;
                }
                uint32_t fd = 0;
                if (!nvOpen(&fd, "/dev/nvdisp-disp1")) {
                    NvdcMode2 display_b = { 0 };
                    if (nvIoctl(fd, NVDISP_GET_MODE2, &display_b) == 0) {
                        if (!display_b.pclkKHz) {
                            nvClose(fd);
                            return false;
                        }

                        if (g_lastVActive != display_b.vActive) {
                            g_lastVActive = display_b.vActive;
                            _getDockedHighestRefreshRate(fd);
                        }

                        uint64_t h_total = display_b.hActive + display_b.hFrontPorch + display_b.hSyncWidth + display_b.hBackPorch;
                        uint64_t v_total = display_b.vActive + display_b.vFrontPorch + display_b.vSyncWidth + display_b.vBackPorch;
                        uint64_t pixelClock = display_b.pclkKHz * 1000 + 999;
                        value = (u32)(pixelClock / (h_total * v_total));
                    } else {
                        value = 60;
                    }
                    nvClose(fd);
                } else {
                    value = 60;
                }
            } else if (!g_config.isRetroSUPER) {
                // Handheld mode
                g_config.isDocked = false;
                g_canChangeRefreshRateDocked = false;

                uint32_t pixelClock = (9375ULL * ((4096 * ((2 * temp.PLLD_DIVN) + 1)) + misc.PLLD_SDM_DIN)) / (8 * temp.PLLD_DIVM);
                value = pixelClock / (DSI_CLOCK_HZ / 60);
            } else {
                return false;
            }
        }

        *out_refreshRate = value;
        return true;
    }

    void Shutdown(void) {
        g_initialized = false;
        memset(&g_config, 0, sizeof(g_config));
    }
}  // namespace display