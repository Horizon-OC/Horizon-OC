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

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "../format.h"
#include "base_menu_gui.h"
#include "fatal_gui.h"
#include "main_gui.h"

#define TOP_Y_OFFSET 15

// Cache hardware model to avoid repeated syscalls

BaseMenuGui::BaseMenuGui()
    : tempColors{
          tsl::Color(0), tsl::Color(0), tsl::Color(0), tsl::Color(0), tsl::Color(0), tsl::Color(0), tsl::Color(0),
      } {
    tsl::initializeThemeVars();
    this->context = nullptr;
    this->lastContextUpdate = 0;
    this->listElement = nullptr;

    // Pre-cache hardware model during initialization
    IsAula();
    IsMariko();
    IsHoag();

    // Initialize display strings
    memset(displayStrings, 0, sizeof(displayStrings));
}

BaseMenuGui::~BaseMenuGui() {
    delete this->context;  // delete handles nullptr automatically
}

// Fast preDraw - just renders pre-computed strings
void BaseMenuGui::preDraw(tsl::gfx::Renderer *renderer) {
    BaseGui::preDraw(renderer);

    // All constants pre-calculated and cached
    const char *labels[] = { "App ID", "Profile", "CPU", "GPU", "MEM",  "SoC", "Board",
                             "Skin",   "Now",     "Avg", "BAT", "PMIC", "Fan", IsAula() || this->context->isUsingRetroSuper ? "OLED" : "LCD",
                             "FPS",    "RES" };

    static constexpr u32 dataPositions[6] = { 63 - 3 + 3, 200 - 1, 344 - 1 - 3, 200 - 1, 342 - 1, 321 - 1 };

    static u32 labelWidths[10];
    static bool positionsInitialized = false;

    if (!positionsInitialized) {
        for (int i = 0; i < 10; i++) {
            labelWidths[i] = renderer->getTextDimensions(labels[i], false, SMALL_TEXT_SIZE).first;
        }
        positionsInitialized = true;
    }
    static u32 positions[10] = { 24 - 1,
                                 310 - labelWidths[1],
                                 24 - 1,
                                 192 - labelWidths[3],
                                 332 - labelWidths[4],
                                 24 - 1,
                                 192 - labelWidths[6],
                                 332 - labelWidths[7],
                                 192 - labelWidths[8],
                                 332 - labelWidths[9] };

    static u32 maxProfileValueWidth = renderer->getTextDimensions("USB Charger", false, SMALL_TEXT_SIZE).first;  // longest word

    u32 y = 91 + TOP_Y_OFFSET;

    // === TOP SECTION ===
    renderer->drawRoundedRect(14, 70 - 1 + TOP_Y_OFFSET, 420, 30 + 2, 12.0f, renderer->aWithOpacity(tsl::tableBGColor));

    // App ID - use pre-formatted string
    renderer->drawString(labels[0], false, positions[0], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    renderer->drawString(displayStrings[0], false, positions[0] + labelWidths[0] + 9, y, SMALL_TEXT_SIZE, tsl::infoTextColor);

    // Profile - use pre-formatted string
    renderer->drawString(labels[1], false, 423 - maxProfileValueWidth - labelWidths[1] - 9, y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    renderer->drawString(displayStrings[1], false, 423 - maxProfileValueWidth, y, SMALL_TEXT_SIZE, tsl::infoTextColor);

    y += 38;  // Direct assignment instead of += 38

    // === MAIN DATA SECTION ===
    renderer->drawRoundedRect(14, 106 + TOP_Y_OFFSET, 420, 156, 10.0f, renderer->aWithOpacity(tsl::tableBGColor));

    // === FREQUENCY SECTION ===
    // Labels first (better cache locality)
    renderer->drawString(labels[2], false, positions[2], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    renderer->drawString(labels[3], false, positions[3], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    renderer->drawString(labels[4], false, positions[4], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    // Current frequencies - use pre-formatted strings
    renderer->drawString(displayStrings[2], false, dataPositions[0], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // CPU
    renderer->drawString(displayStrings[3], false, dataPositions[1], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // GPU
    renderer->drawString(displayStrings[4], false, dataPositions[2], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // MEM

    y += 20;  // Direct assignment (129 + 20)

    renderer->drawString(displayStrings[5], false, dataPositions[0], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // CPU real
    renderer->drawString(displayStrings[6], false, dataPositions[1], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // GPU real
    renderer->drawString(displayStrings[7], false, dataPositions[2], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // MEM real

    renderer->drawString(displayStrings[28], false, positions[2], y, SMALL_TEXT_SIZE, tempColors[HocClkThermalSensor_CPU]);  // CPU Real Temp
    renderer->drawString(displayStrings[29], false, positions[3], y, SMALL_TEXT_SIZE, tempColors[HocClkThermalSensor_GPU]);  // GPU Real Temp
    renderer->drawString(displayStrings[30], false, positions[4], y, SMALL_TEXT_SIZE, tempColors[HocClkThermalSensor_MEM]);  // RAM Real Temp

    // === REAL FREQUENCIES ===

    y += 20;  // Direct assignment (149 + 20)

    // === VOLTAGES ===
    renderer->drawString(displayStrings[8], false, dataPositions[0], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // CPU voltage
    renderer->drawString(displayStrings[9], false, dataPositions[1], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // GPU voltage

    renderer->drawStringWithColoredSections(displayStrings[10], false, { "" }, dataPositions[2], y, SMALL_TEXT_SIZE, tsl::infoTextColor,
                                            tsl::separatorColor);

    renderer->drawString(displayStrings[19], false, positions[2], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // CPU Usage
    renderer->drawString(displayStrings[17], false, positions[3], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // GPU Usage
    renderer->drawString(displayStrings[18], false, positions[4], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // RAM Usage

    y += 22;  // Direct assignment (169 + 22)

    // === TEMPERATURE SECTION ===
    // Labels
    renderer->drawString(labels[5], false, positions[5], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    renderer->drawString(labels[6], false, positions[6], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    renderer->drawString(labels[7], false, positions[7], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);

    // Temperatures with color - use pre-computed colors
    renderer->drawString(displayStrings[11], false, dataPositions[0] - 1, y, SMALL_TEXT_SIZE, tempColors[HocClkThermalSensor_SOC]);  // SOC
    renderer->drawString(displayStrings[12], false, dataPositions[1], y, SMALL_TEXT_SIZE, tempColors[HocClkThermalSensor_PCB]);      // PCB
    renderer->drawString(displayStrings[13], false, dataPositions[2], y, SMALL_TEXT_SIZE, tempColors[HocClkThermalSensor_Skin]);     // Skin

    y += 20;  // Direct assignment (191 + 20)

    renderer->drawString(displayStrings[14], false, dataPositions[0], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // SOC voltage

    // Power labels and values
    renderer->drawString(labels[8], false, positions[8] - 1, y, SMALL_TEXT_SIZE, tsl::sectionTextColor);
    renderer->drawString(labels[9], false, positions[9], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);

    renderer->drawString(displayStrings[15], false, dataPositions[3], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // Power now
    renderer->drawString(displayStrings[16], false, dataPositions[4], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // Power avg

    y += 20;

    renderer->drawString(labels[10], false, positions[2], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);

    renderer->drawString(displayStrings[20], false, dataPositions[0], y, SMALL_TEXT_SIZE, tempColors[HocClkThermalSensor_Battery]);  // Battery

    renderer->drawString(labels[12], false, positions[3], y, SMALL_TEXT_SIZE, tsl::sectionTextColor);  // fan label

    renderer->drawString(displayStrings[24], false, dataPositions[1] + 5, y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // fan speed

    renderer->drawString(labels[13], false, positions[4] + 4, y, SMALL_TEXT_SIZE, tsl::sectionTextColor);  // disp label

    renderer->drawString(displayStrings[25], false, dataPositions[2] + 6, y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // disp freq

    y += 20;

    renderer->drawString(displayStrings[21], false, dataPositions[0], y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // Bat voltage
    renderer->drawString(displayStrings[23], false, positions[2] - 2, y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // Bat Age

    if (this->context->isSaltyNXInstalled) {

        renderer->drawString(labels[15], false, positions[3] + 7, y, SMALL_TEXT_SIZE, tsl::sectionTextColor);           // RES label
        renderer->drawString(displayStrings[27], false, dataPositions[1] + 5, y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // RES

        renderer->drawString(labels[14], false, positions[4] + 9, y, SMALL_TEXT_SIZE, tsl::sectionTextColor);           // FPS label
        renderer->drawString(displayStrings[26], false, dataPositions[2] + 6, y, SMALL_TEXT_SIZE, tsl::infoTextColor);  // FPS
    }

    y += 20;
}

// Optimized refresh - now does all the string formatting once per second
void BaseMenuGui::refresh() {
    const u64 ticks = armGetSystemTick();
    // Use cached comparison - 1 billion nanoseconds
    if (armTicksToNs(ticks - this->lastContextUpdate) <= 1000000000UL) [[likely]] {
        return;  // Early exit for most calls
    }

    this->lastContextUpdate = ticks;

    // Lazy context allocation
    if (!this->context) [[unlikely]] {
        this->context = new HocClkContext;
    }

    Result rc = hocclkIpcGetCurrentContext(this->context);
    if (R_FAILED(rc)) [[unlikely]] {
        FatalGui::openWithResultCode("hocclkIpcGetCurrentContext", rc);
        return;
    }

    rc = hocclkIpcGetConfigValues(&configList);
    if (R_FAILED(rc)) [[unlikely]] {
        FatalGui::openWithResultCode("hocclkIpcGetConfigValues", rc);
        return;
    }
    // dockedHighestAllowedRefreshRate = this->context->maxDisplayFreq;

    // === FORMAT ALL DISPLAY STRINGS (once per second) ===
    // App ID (hex conversion)
    sprintf(displayStrings[0], "%016lX", context->applicationId);

    // Profile
    strcpy(displayStrings[1], hocclkFormatProfile(context->profile, true));

    // Current frequencies
    u32 hz = context->freqs[HocClkModule_CPU];  // CPU
    sprintf(displayStrings[2], "%u.%u MHz", hz / 1000000U, (hz / 100000U) % 10U);

    hz = context->freqs[HocClkModule_GPU];  // GPU
    sprintf(displayStrings[3], "%u.%u MHz", hz / 1000000U, (hz / 100000U) % 10U);

    hz = context->freqs[HocClkModule_MEM];  // MEM
    std::uint32_t unit = configList.values[HocClkConfigValue_RamDisplayUnit];
    std::uint32_t mhz = hz / 1000000U;
    std::uint32_t mts = mhz * 2;
    std::uint32_t tenth = (hz / 100000U) % 10U;
    if (unit == RamDisplayUnit_MTs)
        sprintf(displayStrings[4], "%u MT/s", mts);
    else if (unit == RamDisplayUnit_MHz)
        sprintf(displayStrings[4], "%u.%u MHz", mhz, tenth);
    else if (unit == RamDisplayUnit_MHzMTs) {
        hz = context->realFreqs[HocClkModule_MEM];
        mhz = hz / 1000000U;
        tenth = (hz / 100000U) % 10U;
        sprintf(displayStrings[4], "%u.%u MHz", mhz, tenth);
    }

    // Real frequencies
    hz = context->realFreqs[HocClkModule_CPU];  // CPU
    sprintf(displayStrings[5], "%u.%u MHz", hz / 1000000U, (hz / 100000U) % 10U);

    hz = context->realFreqs[HocClkModule_GPU];  // GPU
    sprintf(displayStrings[6], "%u.%u MHz", hz / 1000000U, (hz / 100000U) % 10U);

    hz = context->realFreqs[HocClkModule_MEM];  // MEM
    unit = configList.values[HocClkConfigValue_RamDisplayUnit];
    mhz = hz / 1000000U;
    mts = mhz * 2;
    tenth = (hz / 100000U) % 10U;
    if (unit == RamDisplayUnit_MTs || unit == RamDisplayUnit_MHzMTs)
        sprintf(displayStrings[7], "%u MT/s", mts);
    else
        sprintf(displayStrings[7], "%u.%u MHz", mhz, tenth);

    // Voltages
    sprintf(displayStrings[8], "%.1f mV", context->voltages[HocClkVoltage_CPU] / 1000.0);
    sprintf(displayStrings[9], "%.1f mV", context->voltages[HocClkVoltage_GPU] / 1000.0);

    switch (configList.values[HocClkConfigValue_RAMVoltDisplayMode]) {
        case RamDisplayMode_VDD2:
            sprintf(displayStrings[10], "%u.%u mV", context->voltages[HocClkVoltage_EMCVDD2] / 1000U,
                    (context->voltages[HocClkVoltage_EMCVDD2] % 1000U) / 100U);
            break;
        case RamDisplayMode_VDDQ:
            sprintf(displayStrings[10], "%u.%u mV", context->voltages[HocClkVoltage_EMCVDDQ] / 1000U,
                    (context->voltages[HocClkVoltage_EMCVDDQ] % 1000U) / 100U);
            break;
        case RamDisplayMode_VDD2VDDQ:
            sprintf(displayStrings[10], "%u/%u mV", context->voltages[HocClkVoltage_EMCVDD2] / 1000U,
                    context->voltages[HocClkVoltage_EMCVDDQ] / 1000U);
            break;
        default:
            strcpy(displayStrings[10], "N/A");
            break;
    }

    // Temperatures and pre-compute colors
    u32 millis = context->temps[HocClkThermalSensor_SOC];  // SOC
    sprintf(displayStrings[11], "%u.%u °C", millis / 1000U, (millis % 1000U) / 100U);
    tempColors[HocClkThermalSensor_SOC] = tsl::GradientColor(millis * 0.001f);

    millis = context->temps[HocClkThermalSensor_PCB];  // PCB
    sprintf(displayStrings[12], "%u.%u °C", millis / 1000U, (millis % 1000U) / 100U);
    tempColors[HocClkThermalSensor_PCB] = tsl::GradientColor(millis * 0.001f);

    millis = context->temps[HocClkThermalSensor_Skin];  // Skin
    sprintf(displayStrings[13], "%u.%u °C", millis / 1000U, (millis % 1000U) / 100U);
    tempColors[HocClkThermalSensor_Skin] = tsl::GradientColor(millis * 0.001f);

    // SOC voltage (if available)
    sprintf(displayStrings[14], "%u mV", context->voltages[HocClkVoltage_SOC] / 1000U);

    // Power
    sprintf(displayStrings[15], "%d mW", context->power[0]);  // Now
    sprintf(displayStrings[16], "%d mW", context->power[1]);  // Avg

    sprintf(displayStrings[17], "%u%%", context->partLoad[HocClkPartLoad_GPU] / 10);
    sprintf(displayStrings[18], "%u%%", context->partLoad[HocClkPartLoad_EMC] / 10);
    sprintf(displayStrings[19], "%u%%", context->partLoad[HocClkPartLoad_CPUMax] / 10);

    millis = context->temps[HocClkThermalSensor_Battery];  // Battery
    sprintf(displayStrings[20], "%u.%u °C", millis / 1000U, (millis % 1000U) / 100U);
    tempColors[HocClkThermalSensor_Battery] = tsl::GradientColor(millis * 0.001f);

    sprintf(displayStrings[21], "%d mV", context->voltages[HocClkVoltage_Battery]);  // BAT AVG

    sprintf(displayStrings[23], "%u%%", context->partLoad[HocClkPartLoad_BAT] / 1000);

    sprintf(displayStrings[24], "%u%%", context->partLoad[HocClkPartLoad_FAN]);

    sprintf(displayStrings[25], "%u Hz", context->realFreqs[HocClkModule_Display]);
    if (this->context->isSaltyNXInstalled) {
        if (context->fps == 254) {
            strcpy(displayStrings[26], "N/A");
        } else {
            memset(displayStrings[26], 0, sizeof(displayStrings[26]));
            sprintf(displayStrings[26], "%u", context->fps);
        }
    }

    if (this->context->isSaltyNXInstalled) {
        if (context->resolutionHeight == 0) {
            strcpy(displayStrings[27], "N/A");
        } else {
            memset(displayStrings[27], 0, sizeof(displayStrings[27]));
            sprintf(displayStrings[27], "%up", context->resolutionHeight);
        }
    }

    millis = context->temps[HocClkThermalSensor_CPU];
    sprintf(displayStrings[28], "%u.%u", millis / 1000U, (millis % 1000U) / 100U);
    tempColors[HocClkThermalSensor_CPU] = tsl::GradientColor(millis * 0.001f);

    millis = context->temps[HocClkThermalSensor_GPU];
    sprintf(displayStrings[29], "%u.%u", millis / 1000U, (millis % 1000U) / 100U);
    tempColors[HocClkThermalSensor_GPU] = tsl::GradientColor(millis * 0.001f);

    millis = context->temps[HocClkThermalSensor_MEM];
    sprintf(displayStrings[30], "%u.%u", millis / 1000U, (millis % 1000U) / 100U);
    tempColors[HocClkThermalSensor_MEM] = tsl::GradientColor(millis * 0.001f);
}

tsl::elm::Element *BaseMenuGui::baseUI() {
    auto *list = new tsl::elm::List();
    this->listElement = list;
    this->listUI();

    return list;
}

std::string BaseMenuGui::getJumpToItemName() {
    return "";
}

bool BaseMenuGui::handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState leftJoyStick,
                               HidAnalogStickState rightJoyStick) {
    if (keysDown & KEY_B) {
        triggerExitFeedback();
        lastItemName = this->getJumpToItemName();
        tsl::swapTo<MainGui>();
        return true;
    }
    return false;
}
