/*
 *
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
 *
 */

#include "ult_ext.h"
#include "../format.h"
#include "fatal_gui.h"
#include "global_override_gui.h"
#include "value_choice_gui.h"
#include "labels.h"

GlobalOverrideGui::GlobalOverrideGui()
{
    for (std::uint16_t m = 0; m < HocClkModule_EnumMax; m++) {
        this->listItems[m] = nullptr;
        this->listHz[m] = 0;
    }
}

void GlobalOverrideGui::openFreqChoiceGui(HocClkModule module)
{
    std::uint32_t hzList[HOCCLK_FREQ_LIST_MAX];
    std::uint32_t hzCount;
    Result rc =
    hocclkIpcGetFreqList(module, &hzList[0], HOCCLK_FREQ_LIST_MAX, &hzCount);
    if (R_FAILED(rc)) {
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
    tsl::changeTo<FreqChoiceGui>(
    this->context->overrideFreqs[module], hzList, hzCount, module,
    [this, module](std::uint32_t hz) {
        Result rc = hocclkIpcSetOverride(module, hz);
        if (R_FAILED(rc)) {
            FatalGui::openWithResultCode("hocclkIpcSetOverride", rc);
            return false;
        }

        this->lastContextUpdate = armGetSystemTick();
        this->context->overrideFreqs[module] = hz;

        return true;
    },
    true, labels
    );
}

void GlobalOverrideGui::openValueChoiceGui(
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

void GlobalOverrideGui::addModuleListItemValue(
    HocClkModule module,
    const std::string& categoryName,
    std::uint32_t min,
    std::uint32_t max,
    std::uint32_t step,
    const std::string& suffix,
    std::uint32_t divisor,
    int decimalPlaces,
    ValueThresholds thresholds,
    const std::vector<NamedValue>& namedValues,
    bool showDefaultValue
)
{
    bool hasNamedValues = !namedValues.empty();

    if (!hasNamedValues) {
        this->customFormatModules[module] = std::make_tuple(suffix, divisor, decimalPlaces);
    }

    tsl::elm::ListItem* listItem =
        new tsl::elm::ListItem(hocclkFormatModule(module, true));

    listItem->setValue(FREQ_DEFAULT_TEXT);

    listItem->setClickListener(
        [this,
         listItem,
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
         hasNamedValues,
         showDefaultValue](u64 keys)
        {
            if ((keys & HidNpadButton_A) == HidNpadButton_A)
            {
                if (!this->context) {
                    return false;
                }

                std::uint32_t currentValue =
                    this->context->overrideFreqs[module] * divisor;

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

                    [this, listItem, module, divisor, suffix, decimalPlaces, thresholds, namedValues, hasNamedValues, showDefaultValue](std::uint32_t value) -> bool
                    {
                        if (!this->context) {
                            return false;
                        }

                        this->context->overrideFreqs[module] = value / divisor;
                        this->listHz[module] = value / divisor;

                        if (value == 0) {
                            listItem->setValue(FREQ_DEFAULT_TEXT);
                        } else if (hasNamedValues) {
                            for (const auto& namedValue : namedValues) {
                                if (namedValue.value == value / divisor) {
                                    listItem->setValue(namedValue.name);
                                    break;
                                }
                            }
                        } else {
                            char buf[32];
                            if (decimalPlaces > 0) {
                                double displayValue = (double)value / divisor;
                                snprintf(buf, sizeof(buf), "%.*f%s",
                                        decimalPlaces, displayValue, suffix.c_str());
                            } else {
                                snprintf(buf, sizeof(buf), "%u%s",
                                        value / divisor, suffix.c_str());
                            }
                            listItem->setValue(buf);
                        }

                        Result rc =
                            hocclkIpcSetOverride(module, this->context->overrideFreqs[module]);

                        if (R_FAILED(rc))
                        {
                            FatalGui::openWithResultCode(
                                "hocclkIpcSetOverride", rc);
                            return false;
                        }

                        this->lastContextUpdate = armGetSystemTick();
                        return true;
                    },

                    thresholds,
                    false,
                    std::map<std::uint32_t, std::string>(),
                    namedValues,
                    showDefaultValue
                );

                return true;
            }
            else if ((keys & HidNpadButton_Y) == HidNpadButton_Y)
            {
                if (!this->context) {
                    return false;
                }

                this->context->overrideFreqs[module] = 0;
                this->listHz[module] = 0;
                listItem->setValue(FREQ_DEFAULT_TEXT);

                Result rc = hocclkIpcSetOverride(module, 0);

                if (R_FAILED(rc))
                {
                    FatalGui::openWithResultCode("hocclkIpcSetOverride", rc);
                    return false;
                }

                this->lastContextUpdate = armGetSystemTick();
                return true;
            }

            return false;
        });

    this->listElement->addItem(listItem);
    this->listItems[module] = listItem;
}

void GlobalOverrideGui::addModuleListItem(HocClkModule module)
{
    tsl::elm::ListItem *listItem =
    new tsl::elm::ListItem(hocclkFormatModule(module, true));
    RamDisplayUnit memUnit = (RamDisplayUnit)configList.values[HocClkConfigValue_RamDisplayUnit];
    listItem->setValue(module == HocClkModule_MEM ? formatListFreqMem(0, memUnit) : formatListFreqMHz(0));
    listItem->setClickListener([this, module](u64 keys) {
        if ((keys & HidNpadButton_A) == HidNpadButton_A) {
            this->openFreqChoiceGui(module);
            return true;
        } else if ((keys & HidNpadButton_Y) == HidNpadButton_Y) {
            Result rc = hocclkIpcSetOverride(module, 0);
            if (R_FAILED(rc)) {
                FatalGui::openWithResultCode("hocclkIpcSetOverride", rc);
                return false;
            }

            this->lastContextUpdate = armGetSystemTick();
            this->context->overrideFreqs[module] = 0;
            this->listHz[module] = 0;

            this->listItems[module]->setValue(module == HocClkModule_MEM ? formatListFreqMem(0, (RamDisplayUnit)configList.values[HocClkConfigValue_RamDisplayUnit]) : formatListFreqHz(0));

            return true;
        }
        return false;
    });

    this->listElement->addItem(listItem);
    this->listItems[module] = listItem;
}

void GlobalOverrideGui::addModuleToggleItem(HocClkModule module)
{
    const char *moduleName = hocclkFormatModule(module, true);
    bool isOn = this->listHz[module];

    tsl::elm::ToggleListItem *toggle =
    new tsl::elm::ToggleListItem(moduleName, isOn);

    toggle->setStateChangedListener([this, module, toggle](bool state) {
        Result rc = hocclkIpcSetOverride(module, state ? 1 : 0);
        if (R_FAILED(rc)) {
            FatalGui::openWithResultCode("hocclkIpcSetProfiles", rc);
        }
        this->lastContextUpdate = armGetSystemTick();
        this->context->overrideFreqs[module] = 0;
        this->listHz[module] = 0;
    });
    this->listElement->addItem(toggle);
    this->listItems[module] = toggle;
}

class GovernorOverrideSubMenuGui : public BaseMenuGui {
    u32 packed;
public:
    GovernorOverrideSubMenuGui(u32 initialPacked) : packed(initialPacked) {}

    bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState leftJoyStick,
                      HidAnalogStickState rightJoyStick) override {
        return false;
    }

    void listUI() override {
        BaseMenuGui::refresh(); // get latest context
        if(!this->context)
            return;
        Result rc = hocclkIpcGetConfigValues(&configList); // idk why this is needed, probably some refreshing issue
        if (R_FAILED(rc)) [[unlikely]] {
            FatalGui::openWithResultCode("hocclkIpcGetConfigValues", rc);
            return;
        }
        this->listElement->addItem(new tsl::elm::CategoryHeader("Governor"));

        static constexpr struct { const char* label; int shift; } kAll[] = {
            {"CPU", 0}, {"GPU", 8}, {"VRR", 16}
        };
        int count = configList.values[HocClkConfigValue_OverwriteRefreshRate] && this->context->profile != HocClkProfile_Docked ? 3 : 2;

        for (int i = 0; i < count; i++) {
            u8 cur = (this->packed >> kAll[i].shift) & 0xFF;
            auto* bar = new tsl::elm::NamedStepTrackBar(
                "", {"Do Not Override", "Disabled", "Enabled"},
                true, kAll[i].label
            );
            bar->setProgress(cur);
            int shift = kAll[i].shift;
            bar->setValueChangedListener([this, shift](u8 value) {
                this->packed = (this->packed & ~(0xFFu << shift)) | ((u32)value << shift);
                Result rc = hocclkIpcSetOverride(HocClkModule_Governor, this->packed);
                if (R_FAILED(rc)) FatalGui::openWithResultCode("hocclkIpcSetOverride", rc);
                this->lastContextUpdate = armGetSystemTick();
            });
            this->listElement->addItem(bar);
        }
    }
};

void GlobalOverrideGui::addGovernorSection() {
    auto* item = new tsl::elm::ListItem("Governor");
    item->setValue("\u2192"); // right arrow
    item->setClickListener([this](u64 keys) {
        if (keys & HidNpadButton_A) {
            u32 packed = this->context ? this->context->overrideFreqs[HocClkModule_Governor] : 0;
            tsl::changeTo<GovernorOverrideSubMenuGui>(packed);
            return true;
        }
        return false;
    });
    this->listElement->addItem(item);
}

void GlobalOverrideGui::listUI()
{
    BaseMenuGui::refresh(); // get latest context
    if(!this->context)
        return;

    Result rc = hocclkIpcGetConfigValues(&configList); // idk why this is needed, probably some refreshing issue
    if (R_FAILED(rc)) [[unlikely]] {
        FatalGui::openWithResultCode("hocclkIpcGetConfigValues", rc);
        return;
    }

    this->listElement->addItem(new tsl::elm::CategoryHeader(
    "Temporary Overrides " + ult::DIVIDER_SYMBOL + " \ue0e3 Reset"));
    this->addModuleListItem(HocClkModule_CPU);
    this->addModuleListItem(HocClkModule_GPU);
    this->addModuleListItem(HocClkModule_MEM);
    #if IS_MINIMAL == 0
        ValueThresholds lcdThresholds(60, 65);
        if(configList.values[HocClkConfigValue_OverwriteRefreshRate])
            this->addModuleListItemValue(HocClkModule_Display, "Display", IsAula() ? 45 : 40, configList.values[HocClkConfigValue_MaxDisplayClockH], 1, " Hz", 1, 0, lcdThresholds);
    #endif

    this->addGovernorSection();
}

void GlobalOverrideGui::refresh()
{
    BaseMenuGui::refresh();

    if (!this->context)
        return;

    for (std::uint16_t m = 0; m < HocClkModule_EnumMax; m++) {
        if (m == HocClkModule_Governor) {
            this->listHz[m] = this->context->overrideFreqs[m];
            continue;
        }

        if (this->listItems[m] != nullptr &&
            this->listHz[m] != this->context->overrideFreqs[m]) {

            auto it = this->customFormatModules.find((HocClkModule)m);
            if (it != this->customFormatModules.end()) {
                std::string suffix = std::get<0>(it->second);
                std::uint32_t divisor = std::get<1>(it->second);
                int decimalPlaces = std::get<2>(it->second);

                if (this->context->overrideFreqs[m] == 0) {
                    this->listItems[m]->setValue(FREQ_DEFAULT_TEXT);
                } else {
                    char buf[32];
                    if (decimalPlaces > 0) {
                        double displayValue = (double)this->context->overrideFreqs[m] / divisor;
                        snprintf(buf, sizeof(buf), "%.*f%s",
                                decimalPlaces, displayValue, suffix.c_str());
                    } else {
                        snprintf(buf, sizeof(buf), "%u%s",
                                this->context->overrideFreqs[m] / divisor, suffix.c_str());
                    }
                    this->listItems[m]->setValue(buf);
                }
            } else {
                this->listItems[m]->setValue(
                    m == HocClkModule_MEM
                        ? formatListFreqHzMem(this->context->overrideFreqs[m], (RamDisplayUnit)configList.values[HocClkConfigValue_RamDisplayUnit])
                        : formatListFreqHz(this->context->overrideFreqs[m]));
            }

            this->listHz[m] = this->context->overrideFreqs[m];
        }
    }
}

std::string GlobalOverrideGui::getJumpToItemName() {
    return "Temporary Overrides";
}
