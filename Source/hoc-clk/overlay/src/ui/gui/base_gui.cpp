/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "../elements/base_frame.h"
#include "base_gui.h"


void BaseFrame::draw(tsl::gfx::Renderer *renderer) {
    tsl::elm::HeaderOverlayFrame::draw(renderer);
    this->gui->preDraw(renderer);
}

#include <math.h>
#include <tesla.hpp>


#define LOGO_Y_REAL 65

#define LOGO_X 20
#define LOGO_Y 60
#define LOGO_LABEL_FONT_SIZE 45

#define TEXT_Y 57

#define LOGO_IMG_W 50
#define LOGO_IMG_H 50
#define LOGO_IMG_PAD 8
#define LOGO_TEXT_X (LOGO_X + LOGO_IMG_W + LOGO_IMG_PAD)

#define VERSION_X (LOGO_TEXT_X + 185)
#define VERSION_Y (LOGO_Y - 40)
#define VERSION_FONT_SIZE 15

extern "C" {
extern const u8 hoc_rgba[];
extern const u32 hoc_rgba_size;
}

std::string getVersionString() {
    char buf[0x100] = "";
    Result rc = hocclkIpcGetVersionString(buf, sizeof(buf));
    if (R_FAILED(rc) || buf[0] == '\0') {
        return "Unknown";
    }
    return std::string(buf);
}

static constexpr tsl::Color dynamicLogoRGB1 = tsl::Color(7, 15, 15, 15);
static constexpr tsl::Color dynamicLogoRGB2 = tsl::Color(2, 8, 11, 15);
static constexpr tsl::Color STATIC_TEAL = tsl::Color(7, 15, 15, 15);
const std::string name = " Horizon OC";

static s32 drawDynamicUltraText(tsl::gfx::Renderer *renderer, s32 startX, s32 y, u32 fontSize, const tsl::Color &staticColor,
                                bool useNotificationMethod = false) {
    static constexpr double cycleDuration = 1.6;

    s32 currentX = startX;

    const u64 currentTime_ns = armTicksToNs(armGetSystemTick());
    const double timeNow = static_cast<double>(currentTime_ns) / 1e9;

    const double waveScale = 2.0 * M_PI / cycleDuration;

    for (size_t i = 0; i < name.size(); i++) {
        char letter = name[i];
        if (letter == '\0')
            break;

        double phase = waveScale * (timeNow + i * 0.12);

        double raw = cos(phase);
        double n = (raw + 1.0) * 0.5;
        double s1 = n * n * (3.0 - 2.0 * n);
        double blend = std::clamp(s1, 0.0, 1.0);

        double glow = (cos(phase * 1.5) + 1.0) * 0.5;
        double brightness = 0.75 + glow * 0.25;

        u8 r = static_cast<u8>(((int)dynamicLogoRGB1.r + ((int)dynamicLogoRGB2.r - (int)dynamicLogoRGB1.r) * blend) * brightness);
        u8 g = static_cast<u8>(((int)dynamicLogoRGB1.g + ((int)dynamicLogoRGB2.g - (int)dynamicLogoRGB1.g) * blend) * brightness);
        u8 b = static_cast<u8>(((int)dynamicLogoRGB1.b + ((int)dynamicLogoRGB2.b - (int)dynamicLogoRGB1.b) * blend) * brightness);

        r = std::clamp<u8>(r, 0, 15);
        g = std::clamp<u8>(g, 0, 15);
        b = std::clamp<u8>(b, 0, 15);

        // bool lightning = (fmod(timeNow, 5.0) < 0.15);
        // if (lightning) {
        //     r = std::min<u8>(r + 4, 15);
        //     g = std::min<u8>(g + 4, 15);
        //     b = std::min<u8>(b + 15, 15);
        // }

        tsl::Color color(r, g, b, 15);

        std::string ls(1, letter);

        if (useNotificationMethod)
            currentX += renderer->drawNotificationString(ls, false, currentX, y, fontSize, color).first;
        else
            currentX += renderer->drawString(ls, false, currentX, y, fontSize, color).first;
    }

    return currentX;
}

void BaseGui::preDraw(tsl::gfx::Renderer *renderer) {
    renderer->drawBitmap(LOGO_X, LOGO_Y_REAL - LOGO_LABEL_FONT_SIZE, LOGO_IMG_W, LOGO_IMG_H, hoc_rgba);

    drawDynamicUltraText(renderer, LOGO_TEXT_X, TEXT_Y, LOGO_LABEL_FONT_SIZE, STATIC_TEAL, false);

    static const std::string versionStr = "Version " + getVersionString() + "  \"Gaea\"";
    static constexpr tsl::Color versionColor(9, 9, 9, 15);
    static constexpr s32 vx = LOGO_TEXT_X + 15;
    static constexpr s32 vy = TEXT_Y + 18;
    static constexpr s32 fs = 15;
    static constexpr s32 skew = 3;
    static constexpr s32 passes = 25;
    for (s32 i = 0; i < passes; i++) {
        s32 sliceY = (vy - fs) + i * fs / passes;
        s32 sliceH = fs / passes + 1;
        s32 xOff = skew - (skew * i / (passes - 1));
        renderer->enableScissoring(0, sliceY, tsl::cfg::FramebufferWidth, sliceH);
        renderer->drawString(versionStr.c_str(), false, vx + xOff, vy, fs, versionColor);
        renderer->disableScissoring();
    }
}

tsl::elm::Element *BaseGui::createUI() {
    BaseFrame *rootFrame = new BaseFrame(this, this->headerHeight());
    rootFrame->setContent(this->baseUI());
    return rootFrame;
}

void BaseGui::update() {
    this->refresh();
}