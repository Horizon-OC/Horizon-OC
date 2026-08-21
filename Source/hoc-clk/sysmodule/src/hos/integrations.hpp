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
 *
 */

#pragma once

#include <hocclk.h>
#include <switch.h>

namespace integrations {

    struct NxFpsSharedBlock {
        uint32_t MAGIC;
        uint8_t FPS;
        float FPSavg;
        bool pluginActive;
        uint8_t FPSlocked;
        uint8_t FPSmode;
        uint8_t ZeroSync;
        uint8_t patchApplied;
        uint8_t API;
        uint32_t FPSticks[10];
        uint8_t Buffers;
        uint8_t SetBuffers;
        uint8_t ActiveBuffers;
        uint8_t SetActiveBuffers;
        union {
            struct {
                bool handheld : 1;
                bool docked : 1;
                unsigned int reserved : 6;
            } NX_PACKED ds;
            uint8_t general;
        } displaySync;
        struct resolutionCalls {
            uint16_t width;
            uint16_t height;
            uint16_t calls;
        } renderCalls[8], viewportCalls[8];
        bool forceOriginalRefreshRate;
        bool dontForce60InDocked;
        bool forceSuspend;
        uint8_t currentRefreshRate;
        float readSpeedPerSecond;
        uint8_t FPSlockedDocked;
        uint64_t frameNumber;
    } NX_PACKED;

    bool GetSysDockState();
    bool GetSaltyNXState();

    void LoadSaltyNX();
    u8 GetSaltyNXFPS();
    u16 GetSaltyNXResolutionHeight();

}  // namespace integrations
