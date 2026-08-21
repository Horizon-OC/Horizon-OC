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


#include "ult_ext.h"
#include "app_profile_gui.h"

#include "../format.h"
#include "fatal_gui.h"
#include "labels.h"
AppProfileGui::AppProfileGui(std::uint64_t applicationId, HocClkTitleProfileList* profileList)
{
    this->applicationId = applicationId;
    this->profileList = profileList;
}

AppProfileGui::~AppProfileGui()
{
    delete this->profileList;
}

void AppProfileGui::openFreqChoiceGui(tsl::elm::ListItem* listItem, HocClkProfile profile, HocClkModule module)
{
    std::uint32_t hzList[HOCCLK_FREQ_LIST_MAX];
    std::uint32_t hzCount;
    Result rc = hocclkIpcGetFreqList(module, &hzList[0], HOCCLK_FREQ_LIST_MAX, &hzCount);
    if(R_FAILED(rc))
    {
        FatalGui::openWithResultCode("hocclkIpcGetFreqList", rc);
        return;
    }
    std::map<uint32_t, std::string> labels = {};

    if (module == HocClkModule_CPU) {
        bool isUsingUv = IsMariko() ? configList.values[KipConfigValue_marikoCpuUVHigh] : configList.values[KipConfigValue_eristaCpuUV];
        labels = IsMariko() ? (isUsingUv ? cpu_freq_label_m_uv : cpu_freq_label_m) : (isUsingUv ? cpu_freq_label_e_uv : cpu_freq_label_e);
    } else if (module == HocClkModule_GPU) {
        labels = IsMariko() ? *(marikoUV[configList.values[KipConfigValue_marikoGpuUV]]) : *(eristaUV[configList.values[KipConfigValue_eristaGpuUV]]);
    }
    RamDisplayUnit memUnit = (RamDisplayUnit)configList.values[HocClkConfigValue_RamDisplayUnit];
    tsl::changeTo<FreqChoiceGui>(this->profileList->mhzMap[profile][module] * 1000000, hzList, hzCount, module, [this, listItem, profile, module, memUnit](std::uint32_t hz) {
        this->profileList->mhzMap[profile][module] = hz / 1000000;
        std::uint32_t mhz = this->profileList->mhzMap[profile][module];
        listItem->setValue(module == HocClkModule_MEM ? formatListFreqMem(mhz, memUnit) : formatListFreqMHz(mhz));
        Result rc = hocclkIpcSetProfiles(this->applicationId, this->profileList);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("hocclkIpcSetProfiles", rc);
            return false;
        }

        return true;
    }, true, labels
    );
}

void AppProfileGui::openValueChoiceGui(
    tsl::elm::ListItem* listItem,
    std::uint32_t currentValue,
    const ValueRange& range,
    const std::string& categoryName,
    ValueChoiceListener listener,
    const ValueThresholds& thresholds,
    bool enableThresholds,
    const std::map<std::uint32_t, std::string>& labels,
    const std::vector<NamedValue>& namedValues,
    bool showDefaultValue
)
{
    tsl::changeTo<ValueChoiceGui>(
        currentValue,
        range,
        categoryName,
        listener,
        thresholds,
        enableThresholds,
        labels,
        namedValues,
        showDefaultValue,
        true
    );
}

void AppProfileGui::addModuleListItem(HocClkProfile profile, HocClkModule module)
{
    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(hocclkFormatModule(module, true));
    RamDisplayUnit memUnit = (RamDisplayUnit)configList.values[HocClkConfigValue_RamDisplayUnit];
    std::uint32_t mhz = this->profileList->mhzMap[profile][module];
    listItem->setValue(module == HocClkModule_MEM ? formatListFreqMem(mhz, memUnit) : formatListFreqMHz(mhz));
    listItem->setClickListener([this, listItem, profile, module, memUnit](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A)
        {
            this->openFreqChoiceGui(listItem, profile, module);
            return true;
        }
        else if((keys & HidNpadButton_Y) == HidNpadButton_Y)
        {
            // Reset to "Default" (0 MHz)
            this->profileList->mhzMap[profile][module] = 0;
            listItem->setValue(module == HocClkModule_MEM ? formatListFreqMem(0, memUnit) : formatListFreqMHz(0));

            Result rc = hocclkIpcSetProfiles(this->applicationId, this->profileList);
            if(R_FAILED(rc))
            {
                FatalGui::openWithResultCode("hocclkIpcSetProfiles", rc);
                return false;
            }
            return true;
        }
        return false;
    });
    this->listElement->addItem(listItem);
}

void AppProfileGui::addModuleListItemToggle(HocClkProfile profile, HocClkModule module)
{
    const char* moduleName = hocclkFormatModule(module, true);
    std::uint32_t currentValue = this->profileList->mhzMap[profile][module];

    tsl::elm::ToggleListItem* toggle = new tsl::elm::ToggleListItem(moduleName, currentValue != 0);

    toggle->setStateChangedListener([this, profile, module](bool state) {
        this->profileList->mhzMap[profile][module] = state ? 1 : 0;

        Result rc = hocclkIpcSetProfiles(this->applicationId, this->profileList);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("hocclkIpcSetProfiles", rc);
        }
    });

    this->listElement->addItem(toggle);
}

std::string AppProfileGui::formatValueDisplay(
    std::uint32_t value,
    const std::vector<NamedValue>& namedValues,
    const std::string& suffix,
    std::uint32_t divisor,
    int decimalPlaces
)
{
    if (value == 0) {
        return FREQ_DEFAULT_TEXT;
    }

    if (!namedValues.empty()) {
        for (const auto& namedValue : namedValues) {
            if (namedValue.value == value) {
                return namedValue.name;
            }
        }
    }

    char buf[32];
    if (decimalPlaces > 0) {
        double displayValue = (double)value / divisor;
        snprintf(buf, sizeof(buf), "%.*f%s", decimalPlaces, displayValue, suffix.c_str());
    } else {
        snprintf(buf, sizeof(buf), "%u%s", value / divisor, suffix.c_str());
    }
    return std::string(buf);
}

void AppProfileGui::addModuleListItemValue(
    HocClkProfile profile,
    HocClkModule module,
    const std::string& categoryName,
    std::uint32_t min,
    std::uint32_t max,
    std::uint32_t step,
    const std::string& suffix,
    std::uint32_t divisor,
    int decimalPlaces,
    ValueThresholds thresholds,
    std::vector<NamedValue> namedValues,
    bool showDefaultValue
)
{
    tsl::elm::ListItem* listItem =
        new tsl::elm::ListItem(hocclkFormatModule(module, true));
    std::uint32_t storedValue = this->profileList->mhzMap[profile][module];

    listItem->setValue(this->formatValueDisplay(storedValue, namedValues, suffix, divisor, decimalPlaces));

    listItem->setClickListener(
        [this,
         listItem,
         profile,
         module,
         categoryName,
         min,
         max,
         step,
         suffix,
         divisor,
         decimalPlaces,
         thresholds,
         namedValues,
         showDefaultValue](u64 keys)
        {
            if ((keys & HidNpadButton_A) == HidNpadButton_A)
            {
                std::uint32_t currentValue =
                    this->profileList->mhzMap[profile][module] * divisor;
                ValueRange range(
                    min,
                    max,
                    step,
                    suffix,
                    divisor,
                    decimalPlaces
                );
                this->openValueChoiceGui(
                    listItem,
                    currentValue,
                    range,
                    categoryName,
                    [this, listItem, profile, module, divisor, suffix, decimalPlaces, thresholds, namedValues](std::uint32_t value) -> bool
                    {
                        this->profileList->mhzMap[profile][module] = value / divisor;
                        listItem->setValue(this->formatValueDisplay(value / divisor, namedValues, suffix, divisor, decimalPlaces));

                        Result rc =
                            hocclkIpcSetProfiles(this->applicationId,
                                                 this->profileList);
                        if (R_FAILED(rc))
                        {
                            FatalGui::openWithResultCode(
                                "hocclkIpcSetProfiles", rc);
                            return false;
                        }
                        return true;
                    },
                    thresholds,
                    false,
                    {},
                    namedValues,
                    showDefaultValue
                );
                return true;
            }
            else if ((keys & HidNpadButton_Y) == HidNpadButton_Y)
            {
                this->profileList->mhzMap[profile][module] = 0;
                listItem->setValue(FREQ_DEFAULT_TEXT);
                Result rc =
                    hocclkIpcSetProfiles(this->applicationId,
                                         this->profileList);
                if (R_FAILED(rc))
                {
                    FatalGui::openWithResultCode("hocclkIpcSetProfiles", rc);
                    return false;
                }
                return true;
            }
            return false;
        });
    this->listElement->addItem(listItem);
}

class GovernorProfileSubMenuGui : public BaseMenuGui {
    uint64_t applicationId;
    HocClkTitleProfileList* profileList;
    HocClkProfile profile;
public:
    GovernorProfileSubMenuGui(uint64_t appId, HocClkTitleProfileList* pList, HocClkProfile prof)
        : applicationId(appId), profileList(pList), profile(prof) {}

    bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState leftJoyStick,
                      HidAnalogStickState rightJoyStick) override {
        return false;
    }

    void listUI() override {
        BaseMenuGui::refresh(); // get latest context
        if(!this->context)
            return;
        Result rc = hocclkIpcGetConfigValues(&configList);
        if (R_FAILED(rc)) [[unlikely]] {
            FatalGui::openWithResultCode("hocclkIpcGetConfigValues", rc);
            return;
        }
        this->listElement->addItem(new tsl::elm::CategoryHeader("Governor"));

        static constexpr struct { const char* label; int shift; } kAll[] = {
            {"CPU", 0}, {"GPU", 8}, {"VRR", 16}
        };
        int count = configList.values[HocClkConfigValue_OverwriteRefreshRate] && profile != HocClkProfile_Docked ? 3 : 2;

        for (int i = 0; i < count; i++) {
            u8 cur = (this->profileList->mhzMap[this->profile][HocClkModule_Governor] >> kAll[i].shift) & 0xFF;
            auto* bar = new tsl::elm::NamedStepTrackBar(
                "", {"Do Not Override", "Disabled", "Enabled"},
                true, kAll[i].label
            );
            bar->setProgress(cur);
            int shift = kAll[i].shift;
            bar->setValueChangedListener([this, shift](u8 value) {
                u32& packed = this->profileList->mhzMap[this->profile][HocClkModule_Governor];
                packed = (packed & ~(0xFFu << shift)) | ((u32)value << shift);
                Result rc = hocclkIpcSetProfiles(this->applicationId, this->profileList);
                if (R_FAILED(rc)) FatalGui::openWithResultCode("hocclkIpcSetProfiles", rc);
            });
            this->listElement->addItem(bar);
        }
    }
};

void AppProfileGui::addGovernorSection(HocClkProfile profile) {
    auto* item = new tsl::elm::ListItem("Governor");
    item->setValue("\u2192"); // Right arrow
    item->setClickListener([this, profile](u64 keys) {
        if (keys & HidNpadButton_A) {
            tsl::changeTo<GovernorProfileSubMenuGui>(
                this->applicationId, this->profileList, profile
            );
            return true;
        }
        return false;
    });
    this->listElement->addItem(item);
}

void AppProfileGui::addProfileUI(HocClkProfile profile)
{
    BaseMenuGui::refresh();
    if(!this->context)
        return;
    Result rc = hocclkIpcGetConfigValues(&configList);
    if (R_FAILED(rc)) [[unlikely]] {
        FatalGui::openWithResultCode("hocclkIpcGetConfigValues", rc);
        return;
    }
    if((profile == HocClkProfile_Docked && IsHoag()) || profile == HocClkProfile_HandheldCharging)
        return;
    this->listElement->addItem(new tsl::elm::CategoryHeader(hocclkFormatProfile(profile, true) + std::string(" ") + ult::DIVIDER_SYMBOL + " \ue0e3 Reset"));
    this->addModuleListItem(profile, HocClkModule_CPU);
    this->addModuleListItem(profile, HocClkModule_GPU);
    this->addModuleListItem(profile, HocClkModule_MEM);
    #if IS_MINIMAL == 0
        ValueThresholds lcdThresholds(60, 65);
        ValueThresholds DThresholdsOLED(120, 500); // nothing is dangerous, past 120hz you can get applet crashes

        if(configList.values[HocClkConfigValue_OverwriteRefreshRate]) {
            if(profile != HocClkProfile_Docked) {
                this->addModuleListItemValue(profile, HocClkModule_Display, "Display", IsAula() ? 45 : 40, configList.values[HocClkConfigValue_MaxDisplayClockH],  1, " Hz", 1, 0, lcdThresholds);
            } else {
                if(IsAula() && this->context->isSysDockInstalled) {
                    std::vector<NamedValue> dockedFreqs = {
                        NamedValue("40 Hz", 40),
                        NamedValue("45 Hz", 45),
                        NamedValue("50 Hz", 50),
                        NamedValue("55 Hz", 55),
                        NamedValue("60 Hz", 60),
                        NamedValue("70 Hz", 70),
                        NamedValue("72 Hz", 72),
                        NamedValue("75 Hz", 75),
                        NamedValue("80 Hz", 80),
                        NamedValue("90 Hz", 90),
                        NamedValue("95 Hz", 95),
                        NamedValue("100 Hz", 100),
                        NamedValue("110 Hz", 110),
                        NamedValue("120 Hz", 120),
                        NamedValue("130 Hz", 130),
                        NamedValue("140 Hz", 140),
                        NamedValue("144 Hz", 144),
                        NamedValue("150 Hz", 150),
                        NamedValue("160 Hz", 160),
                        NamedValue("165 Hz", 165),
                        NamedValue("170 Hz", 170),
                        NamedValue("180 Hz", 180),
                        NamedValue("190 Hz", 190),
                        NamedValue("200 Hz", 200),
                        NamedValue("210 Hz", 210),
                        NamedValue("220 Hz", 220),
                        NamedValue("230 Hz", 230),
                        NamedValue("240 Hz", 240)
                    };

                    this->addModuleListItemValue(profile, HocClkModule_Display, "Display", 40, 240, 1, " Hz", 1, 0, DThresholdsOLED, dockedFreqs);
                } else if (IsAula() && !this->context->isSysDockInstalled) {
                    std::vector<NamedValue> dockedFreqsLimited = {
                        NamedValue("50 Hz", 50),
                        NamedValue("55 Hz", 55),
                        NamedValue("60 Hz", 60),
                        NamedValue("65 Hz", 65),
                        NamedValue("70 Hz", 70),
                        NamedValue("72 Hz", 72),
                        NamedValue("75 Hz", 75)
                    };

                    this->addModuleListItemValue(profile, HocClkModule_Display, "Display", 50, 75, 1, " Hz", 1, 0, DThresholdsOLED, dockedFreqsLimited);
                } else {
                    std::vector<NamedValue> dockedFreqsStandard = {
                        NamedValue("50 Hz", 50),
                        NamedValue("55 Hz", 55),
                        NamedValue("60 Hz", 60),
                        NamedValue("65 Hz", 65),
                        NamedValue("70 Hz", 70),
                        NamedValue("72 Hz", 72),
                        NamedValue("75 Hz", 75),
                        NamedValue("80 Hz", 80),
                        NamedValue("85 Hz", 85),
                        NamedValue("90 Hz", 90),
                        NamedValue("95 Hz", 95),
                        NamedValue("100 Hz", 100),
                        NamedValue("105 Hz", 105),
                        NamedValue("110 Hz", 110),
                        NamedValue("115 Hz", 115),
                        NamedValue("120 Hz", 120)
                    };
                    this->addModuleListItemValue(profile, HocClkModule_Display, "Display", 50, 120, 1, " Hz", 1, 0, ValueThresholds(), dockedFreqsStandard);
                }
            }
        }
    #endif
    this->addGovernorSection(profile);
}

void AppProfileGui::listUI()
{
    this->addProfileUI(HocClkProfile_Docked);
    this->addProfileUI(HocClkProfile_Handheld);
    this->addProfileUI(HocClkProfile_HandheldCharging);
    this->addProfileUI(HocClkProfile_HandheldChargingOfficial);
    this->addProfileUI(HocClkProfile_HandheldChargingUSB);
}

void AppProfileGui::changeTo(std::uint64_t applicationId)
{
    HocClkTitleProfileList* profileList = new HocClkTitleProfileList;
    Result rc = hocclkIpcGetProfiles(applicationId, profileList);
    if(R_FAILED(rc))
    {
        delete profileList;
        FatalGui::openWithResultCode("hocclkIpcGetProfiles", rc);
        return;
    }

    tsl::swapTo<AppProfileGui>(applicationId, profileList);
}

void AppProfileGui::update()
{
    BaseMenuGui::update();

    if((this->context && this->applicationId != this->context->applicationId) &&  this->applicationId != HOCCLK_GLOBAL_PROFILE_TID)
    {
        tsl::changeTo<FatalGui>(
            "Application changed\n\n"
            "\n"
            "The running application changed\n\n"
            "while editing was going on.",
            ""
        );
    }
}

std::string AppProfileGui::getJumpToItemName() {
    return this->applicationId == HOCCLK_GLOBAL_PROFILE_TID ? "Edit Global Profile" : "Edit App Profile";
}
