/*
 * Mode-Specific Configuration Settings
 *
 * Based on actual settings structures, each mode only shows applicable settings:
 *
 * Mini Mode: Refresh Rate, Colors (background, focus_background, separator, category, text),
 *           Toggles, Font Sizes, Elements, DTC Format
 *
 * Micro Mode: Refresh Rate, Colors (background, separator, category, text), Toggles,
 *            Font Sizes, Elements, Text Alignment, Vertical Position (Top/Bottom only), DTC Format
 *
 * Full Mode: Refresh Rate, Toggles (show_real_freqs, show_deltas, etc.),
 *           Horizontal Position (Left/Right only) - NO colors, fonts, or elements
 *
 * FPS Counter: Refresh Rate, Colors (background, text only), Font Sizes,
 *             Horizontal/Vertical Position
 *
 * FPS Graph: Refresh Rate, Colors (8 graph-specific colors), Toggles (show_info only),
 *           Horizontal/Vertical Position - NO fonts
 *
 * Game Resolutions: Refresh Rate, Colors (background, category, text only),
 *                  Horizontal/Vertical Position - NO toggles, fonts, or elements
 */

#pragma once
#include <tesla.hpp>
#include "../Utils.hpp"

#include <unordered_set>

// External variables for navigation
extern std::string jumpItemName;
extern std::string jumpItemValue;
extern std::atomic<bool> jumpItemExactMatch;
static tsl::elm::ListItem* lastSelectedListItem;

// Forward declarations
class ConfiguratorOverlay;
class RefreshRateConfig;
class FontSizeConfig;
class FontSizeSelector;
class ColorConfig;
class ColorSelector;
class AlphaSelector;
class ShowConfig;
class TogglesConfig;
class DTCFormatConfig;

// Helper functions for color manipulation
inline std::string extractColorWithoutAlpha(const std::string& rgba) {
    if (rgba.length() >= 5 && rgba[0] == '#') {
        return rgba.substr(0, 4); // Return #RGB without alpha
    }
    return rgba;
}

inline std::string extractAlphaFromColor(const std::string& rgba) {
    if (rgba.length() == 5 && rgba[0] == '#') {
        return std::string(1, rgba[4]); // Return just the alpha character
    }
    return "9"; // Default alpha
}

inline std::string setAlphaInColor(const std::string& rgba, char alpha) {
    if (rgba.length() >= 4 && rgba[0] == '#') {
        std::string result = rgba.substr(0, 4); // Get #RGB
        result += alpha; // Add new alpha
        return result;
    }
    return rgba;
}

// Alpha Selector for background colors
class AlphaSelector : public tsl::Gui {
private:
    std::string modeName;
    std::string colorKey;
    std::string title;
    bool isMiniMode;
    bool isMicroMode;
    bool isFPSCounterMode;
    bool isFPSGraphMode;
    bool isGameResolutionsMode;

public:
    AlphaSelector(const std::string& mode, const std::string& key, const std::string& displayTitle)
        : modeName(mode), colorKey(key), title(displayTitle) {
            isMiniMode = (mode == "Mini");
            isMicroMode = (mode == "Micro");
            isFPSCounterMode = (mode == "FPS Counter");
            isFPSGraphMode = (mode == "FPS Graph");
            isGameResolutionsMode = (mode == "Game Resolutions");
        }
    ~AlphaSelector() {
        lastSelectedListItem = nullptr;
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader(title));

        std::string section;
        if (isMiniMode) section = "mini";
        else if (isMicroMode) section = "micro";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";
        else if (isGameResolutionsMode) section = "game_resolutions";

        // Get current color value and extract alpha
        std::string currentColor = ult::parseValueFromIniSection(configIniPath, section, colorKey);
        if (currentColor.empty()) {
            currentColor = "#0009"; // Default
        }
        std::string currentAlpha = extractAlphaFromColor(currentColor);

        // Alpha options
        static const std::vector<std::pair<std::string, char>> alphaOptions = {
            {"Transparent", '0'},
            {"10%", '1'},
            {"20%", '3'},
            {"30%", '4'},
            {"40%", '6'},
            {"50%", '8'},
            {"60%", '9'},
            {"70%", 'B'},
            {"80%", 'C'},
            {"90%", 'E'},
            {"Opaque", 'F'}
        };

        for (const auto& option : alphaOptions) {
            auto* alphaItem = new tsl::elm::ListItem(option.first);
            if (currentAlpha[0] == option.second) {
                alphaItem->setValue(ult::CHECKMARK_SYMBOL);
                lastSelectedListItem = alphaItem;
            }
            alphaItem->setClickListener([this, alphaItem, option, section, currentColor](uint64_t keys) {
                if (keys & KEY_A) {
                    // Get current color and update only the alpha
                    std::string color = ult::parseValueFromIniSection(configIniPath, section, colorKey);
                    if (color.empty()) color = "#0009";

                    std::string newColor = setAlphaInColor(color, option.second);
                    ult::setIniFileValue(configIniPath, section, colorKey, newColor);

                    alphaItem->setValue(ult::CHECKMARK_SYMBOL);
                    if (lastSelectedListItem && lastSelectedListItem != alphaItem) {
                        lastSelectedListItem->setValue("");
                    }
                    lastSelectedListItem = alphaItem;
                    return true;
                }
                return false;
            });
            list->addItem(alphaItem);
        }

        list->jumpToItem("", ult::CHECKMARK_SYMBOL, false);

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Alpha");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            jumpItemName = title;
            jumpItemValue = "";
            jumpItemExactMatch = false;

            tsl::swapTo<ColorConfig>(SwapDepth(2), modeName);
            return true;
        }
        return false;
    }
};


// Define available DTC format options
static const std::vector<std::pair<std::string, std::string>> dtcFormats = {
    // Special
    {"Pretty", "%a, %b %d"+ult::DIVIDER_SYMBOL+"%I:%M %p"},
    {"Compact", "%Y%m%d"+ult::DIVIDER_SYMBOL+"%H:%M:%S"},
    {"FileSafe", "%Y-%m-%d"+ult::DIVIDER_SYMBOL+"%H-%M-%S"},
    {"Day+Time", "%a"+ult::DIVIDER_SYMBOL+"%H:%M"},

    // Datetime (default included here)
    {"Date+Time(s)", "%m-%d-%Y"+ult::DIVIDER_SYMBOL+"%H:%M:%S"},           // default
    {"Date+Time AM/PM", "%m-%d-%Y"+ult::DIVIDER_SYMBOL+"%I:%M %p"},
    {"Date+Time(s) AM/PM", "%m-%d-%Y"+ult::DIVIDER_SYMBOL+"%I:%M:%S %p"},
    {"Date+Time EU", "%d/%m/%Y"+ult::DIVIDER_SYMBOL+"%H:%M"},
    {"Date+Time EU AM/PM", "%d/%m/%Y"+ult::DIVIDER_SYMBOL+"%I:%M %p"},
    {"Date+Time(s) EU AM/PM", "%d/%m/%Y"+ult::DIVIDER_SYMBOL+"%I:%M:%S %p"},
    {"Date+Time ISO", "%Y-%m-%dT"+ult::DIVIDER_SYMBOL+"%H:%M:%S"},

    // Time only
    {"Time 24h", "%H:%M"},
    {"Time AM/PM", "%I:%M %p"},
    {"Time(s) 24h", "%H:%M:%S"},
    {"Time(s) AM/PM", "%I:%M:%S %p"},

    // Date only
    {"Date US", "%m-%d-%Y"},
    {"Date EU", "%d/%m/%Y"},
    {"Date ISO", "%Y-%m-%d"},
    {"Date Short", "%m/%d/%y"}
};

// DTC Format Configuration (Mini/Micro only)
class DTCFormatConfig : public tsl::Gui {
private:
    std::string modeName;
    bool isMiniMode;
    bool isMicroMode;

public:
    DTCFormatConfig(const std::string& mode) : modeName(mode) {
        isMiniMode = (mode == "Mini");
        isMicroMode = (mode == "Micro");
    }
    ~DTCFormatConfig() {
        lastSelectedListItem = nullptr;
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("DTC Format"));

        const std::string section = isMiniMode ? "mini" : "micro";
        std::string currentValue = ult::parseValueFromIniSection(configIniPath, section, "dtc_format");

        // Handle default values
        if (currentValue.empty()) {
            currentValue = isMiniMode ? "%m-%d-%Y"+ult::DIVIDER_SYMBOL+"%H:%M:%S" : "%H:%M:%S";
        }


        for (const auto& format : dtcFormats) {
            auto* formatItem = new tsl::elm::ListItem(format.first);
            //formatItem->setValue(format.second);
            if (format.second == currentValue) {
                formatItem->setValue(ult::CHECKMARK_SYMBOL);
                lastSelectedListItem = formatItem;
            }
            formatItem->setClickListener([this, formatItem, format, section](uint64_t keys) {
                if (keys & KEY_A) {
                    ult::setIniFileValue(configIniPath, section, "dtc_format", format.second);
                    formatItem->setValue(ult::CHECKMARK_SYMBOL);
                    if (lastSelectedListItem && lastSelectedListItem != formatItem) {
                        lastSelectedListItem->setValue("");
                    }
                    lastSelectedListItem = formatItem;
                    return true;
                }
                return false;
            });
            list->addItem(formatItem);
        }

        // Jump to currently selected item
        list->jumpToItem("", ult::CHECKMARK_SYMBOL, false);

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "DTC Format");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            jumpItemName = "DTC Format";
            jumpItemValue = "";
            jumpItemExactMatch = false;

            tsl::swapTo<ConfiguratorOverlay>(SwapDepth(2), modeName);
            return true;
        }
        return false;
    }
};

// Toggles Configuration
class TogglesConfig : public tsl::Gui {
private:
    std::string modeName;
    bool isMiniMode;
    bool isMicroMode;
    bool isFullMode;
    bool isFPSGraphMode;
    bool isGameResolutionsMode;
    bool isFPSCounterMode;

public:
    TogglesConfig(const std::string& mode) : modeName(mode) {
        isMiniMode = (mode == "Mini");
        isMicroMode = (mode == "Micro");
        isFullMode = (mode == "Full");
        isFPSGraphMode = (mode == "FPS Graph");
        isGameResolutionsMode = (mode == "Game Resolutions");
        isFPSCounterMode = (mode == "FPS Counter");
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("Toggles"));

        if (isFPSGraphMode) {
            // FPS Graph: show_info and disable_screenshots
            auto* showInfo = new tsl::elm::ToggleListItem("Info", getCurrentShowInfo());
            showInfo->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "fps-graph", "show_info", state ? "true" : "false");
            });
            list->addItem(showInfo);

		    auto* realTemps = new tsl::elm::ToggleListItem("Real Temperatures", getCurrentFPSGraphRealTemps());
			realTemps->setStateChangedListener([this](bool state) {
				ult::setIniFileValue(configIniPath, "fps-graph", "real_temps", state ? "true" : "false");
			});
			list->addItem(realTemps);

            auto* dynamicColors = new tsl::elm::ToggleListItem("Use Dynamic Colors", getCurrentUseDynamicColors());
            dynamicColors->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "fps-graph", "use_dynamic_colors", state ? "true" : "false");
            });
            list->addItem(dynamicColors);

            auto* disableScreenshots = new tsl::elm::ToggleListItem("Disable Screenshots", getCurrentDisableScreenshots("fps-graph"));
            disableScreenshots->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "fps-graph", "disable_screenshots", state ? "true" : "false");
            });
            list->addItem(disableScreenshots);

        } else if (isFullMode) {
            // Full mode: specific full toggles
            auto* realFreqs = new tsl::elm::ToggleListItem("Real Freqs", getCurrentShowRealFreqs());
            realFreqs->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "full", "show_real_freqs", state ? "true" : "false");
            });
            list->addItem(realFreqs);

			auto* realTemps = new tsl::elm::ToggleListItem("Real Temperatures", getCurrentFullRealTemps());
			realTemps->setStateChangedListener([this](bool state) {
				ult::setIniFileValue(configIniPath, "full", "real_temps", state ? "true" : "false");
			});
			list->addItem(realTemps);

            auto* showDeltas = new tsl::elm::ToggleListItem("Deltas", getCurrentShowDeltas());
            showDeltas->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "full", "show_deltas", state ? "true" : "false");
            });
            list->addItem(showDeltas);

            auto* targetFreqs = new tsl::elm::ToggleListItem("Target Freqs", getCurrentShowTargetFreqs());
            targetFreqs->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "full", "show_target_freqs", state ? "true" : "false");
            });
            list->addItem(targetFreqs);

            auto* showFPS = new tsl::elm::ToggleListItem("FPS", getCurrentShowFPS());
            showFPS->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "full", "show_fps", state ? "true" : "false");
            });
            list->addItem(showFPS);

            auto* showRES = new tsl::elm::ToggleListItem("RES", getCurrentShowRES());
            showRES->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "full", "show_res", state ? "true" : "false");
            });
            list->addItem(showRES);

            auto* showRDSD = new tsl::elm::ToggleListItem("Read Speed", getCurrentShowRDSD());
            showRDSD->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "full", "show_read_speed", state ? "true" : "false");
            });
            list->addItem(showRDSD);

            {
                const std::string curRamInfoMode = getCurrentRamInfoMode("full");
                auto* ramInfoModeItem = new tsl::elm::ListItem("RAM Info Mode");
                ramInfoModeItem->setValue(curRamInfoMode);
                ramInfoModeItem->setClickListener([this, ramInfoModeItem](u64 keys) -> bool {
                    if (!(keys & KEY_A)) return false;
                    const std::string cur = ult::parseValueFromIniSection(configIniPath, "full", "ram_info_mode");
                    const std::string next = cur == "Bandwidth" ? "Load" : "Bandwidth";
                    ult::setIniFileValue(configIniPath, "full", "ram_info_mode", next);
                    ramInfoModeItem->setValue(next);
                    return true;
                });
                list->addItem(ramInfoModeItem);
            }

            auto* dynamicColors = new tsl::elm::ToggleListItem("Use Dynamic Colors", getCurrentUseDynamicColors());
            dynamicColors->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "fps-graph", "use_dynamic_colors", state ? "true" : "false");
            });
            list->addItem(dynamicColors);

            auto* disableScreenshots = new tsl::elm::ToggleListItem("Disable Screenshots", getCurrentDisableScreenshots("full"));
            disableScreenshots->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "full", "disable_screenshots", state ? "true" : "false");
            });
            list->addItem(disableScreenshots);

        } else if (isMiniMode || isMicroMode) {
            // Mini/Micro modes: shared toggles
            const std::string section = isMiniMode ? "mini" : "micro";

            auto* realFreqs = new tsl::elm::ToggleListItem("Real Frequencies", getCurrentRealFreqs());
            realFreqs->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "real_freqs", state ? "true" : "false");
            });
            list->addItem(realFreqs);

            auto* realVolts = new tsl::elm::ToggleListItem("Real Voltages", getCurrentRealVolts());
            realVolts->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "real_volts", state ? "true" : "false");
            });
            list->addItem(realVolts);

			auto* realTemps = new tsl::elm::ToggleListItem("Real Temperatures", getCurrentRealTemps());
			realTemps->setStateChangedListener([this, section](bool state) {
				ult::setIniFileValue(configIniPath, section, "real_temps", state ? "true" : "false");
			});
			list->addItem(realTemps);

            auto* showFullCPU = new tsl::elm::ToggleListItem("Full CPU", getCurrentShowFullCPU());
            showFullCPU->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "show_full_cpu", state ? "true" : "false");
            });
            list->addItem(showFullCPU);

            auto* showVDDQ = new tsl::elm::ToggleListItem("VDDQ", getCurrentShowVDDQ());
            showVDDQ->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "show_vddq", state ? "true" : "false");
            });
            list->addItem(showVDDQ);

            auto* showVDD2 = new tsl::elm::ToggleListItem("VDD2", getCurrentShowVDD2());
            showVDD2->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "show_vdd2", state ? "true" : "false");
            });
            list->addItem(showVDD2);

            auto* showFullRes = new tsl::elm::ToggleListItem("Full Resolution", getCurrentShowFullRes());
            showFullRes->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "show_full_res", state ? "true" : "false");
            });
            list->addItem(showFullRes);

            auto* socVoltage = new tsl::elm::ToggleListItem("SOC Voltage", getCurrentShowSOCVoltage());
            socVoltage->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "show_soc_voltage", state ? "true" : "false");
            });
            list->addItem(socVoltage);

            if (isMiniMode) {
                auto* partLoadCPUGPU = new tsl::elm::ToggleListItem("RAM Load CPU/GPU", getCurrentShowpartLoadCPUGPU());
                partLoadCPUGPU->setStateChangedListener([this, section](bool state) {
                    ult::setIniFileValue(configIniPath, section, "show_RAM_load_CPU_GPU", state ? "true" : "false");
                });
                list->addItem(partLoadCPUGPU);
            }

            if (isMiniMode || isMicroMode) {
                const std::string curRamInfoMode = getCurrentRamInfoMode(section);
                auto* ramInfoModeItem = new tsl::elm::ListItem("RAM Info Mode");
                ramInfoModeItem->setValue(curRamInfoMode);
                ramInfoModeItem->setClickListener([this, section, ramInfoModeItem](u64 keys) -> bool {
                    if (!(keys & KEY_A)) return false;
                    const std::string cur = ult::parseValueFromIniSection(configIniPath, section, "ram_info_mode");
                    const std::string next = cur == "Bandwidth" ? "Load" : "Bandwidth";
                    ult::setIniFileValue(configIniPath, section, "ram_info_mode", next);
                    ramInfoModeItem->setValue(next);
                    return true;
                });
                list->addItem(ramInfoModeItem);
            }

            if (isMiniMode || isMicroMode) {
                auto* invertBatteryDisplay = new tsl::elm::ToggleListItem("Invert Battery Display", getCurrentInvertBatteryDisplay());
                invertBatteryDisplay->setStateChangedListener([this, section](bool state) {
                    ult::setIniFileValue(configIniPath, section, "invert_battery_display", state ? "true" : "false");
                });
                list->addItem(invertBatteryDisplay);
            }

            auto* dtcSymbol = new tsl::elm::ToggleListItem("Use DTC Symbol", getCurrentUseDTCSymbol());
            dtcSymbol->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "use_dtc_symbol", state ? "true" : "false");
            });
            list->addItem(dtcSymbol);

            auto* dynamicColors = new tsl::elm::ToggleListItem("Use Dynamic Colors", getCurrentUseDynamicColors());
            dynamicColors->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "use_dynamic_colors", state ? "true" : "false");
            });
            list->addItem(dynamicColors);

            auto* disableScreenshots = new tsl::elm::ToggleListItem("Disable Screenshots", getCurrentDisableScreenshots(section));
            disableScreenshots->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "disable_screenshots", state ? "true" : "false");
            });
            list->addItem(disableScreenshots);

            auto* sleepExit = new tsl::elm::ToggleListItem("Sleep Exit", getCurrentSleepExit(section));
            sleepExit->setStateChangedListener([this, section](bool state) {
                ult::setIniFileValue(configIniPath, section, "sleep_exit", state ? "true" : "false");
            });
            list->addItem(sleepExit);

        } else if (isGameResolutionsMode) {
            // Game Resolutions mode: only disable_screenshots
            auto* disableScreenshots = new tsl::elm::ToggleListItem("Disable Screenshots", getCurrentDisableScreenshots("game_resolutions"));
            disableScreenshots->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "game_resolutions", "disable_screenshots", state ? "true" : "false");
            });
            list->addItem(disableScreenshots);

        } else if (isFPSCounterMode) {
            // FPS Counter mode: only disable_screenshots
            auto* integerCounter = new tsl::elm::ToggleListItem("Use Integer Counter", getCurrentUseIntegerCounter("fps-counter"));
            integerCounter->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "fps-counter", "use_integer_counter", state ? "true" : "false");
            });
            list->addItem(integerCounter);

            // FPS Counter mode: only disable_screenshots
            auto* disableScreenshots = new tsl::elm::ToggleListItem("Disable Screenshots", getCurrentDisableScreenshots("fps-counter"));
            disableScreenshots->setStateChangedListener([this](bool state) {
                ult::setIniFileValue(configIniPath, "fps-counter", "disable_screenshots", state ? "true" : "false");
            });
            list->addItem(disableScreenshots);
        }

        list->jumpToItem(jumpItemName, jumpItemValue, jumpItemExactMatch);
        {
            jumpItemName = "";
            jumpItemValue = "";
            jumpItemExactMatch = false;
        }

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Configuration");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            tsl::goBack();
            return true;
        }
        return false;
    }

private:
    // Helper methods for getting current toggle states
    bool getCurrentShowInfo() {
        std::string value = ult::parseValueFromIniSection(configIniPath, "fps-graph", "show_info");
        if (value.empty()) return true;
        convertToUpper(value);
        return value == "TRUE";
    }

	bool getCurrentFPSGraphRealTemps() {
		std::string value = ult::parseValueFromIniSection(configIniPath, "fps-graph", "real_temps");
		if (value.empty()) return false;
		convertToUpper(value);
		return value == "TRUE";
	}

    bool getCurrentRealFreqs() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "real_freqs");
        if (value.empty()) return true;
        convertToUpper(value);
        return value == "TRUE";
    }

    bool getCurrentRealVolts() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "real_volts");
        if (value.empty()) return true;
        convertToUpper(value);
        return value == "TRUE";
    }

	bool getCurrentRealTemps() {
		const std::string section = isMiniMode ? "mini" : "micro";
		std::string value = ult::parseValueFromIniSection(configIniPath, section, "real_temps");
		if (value.empty()) return true;
		convertToUpper(value);
		return value == "TRUE";
	}

    bool getCurrentShowFullCPU() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "show_full_cpu");
        if (value.empty()) return false;
        convertToUpper(value);
        return value == "TRUE";
    }

    bool getCurrentShowVDDQ() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "show_vddq");
        if (value.empty()) return false;
        convertToUpper(value);
        return value == "TRUE";
    }

    bool getCurrentShowVDD2() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "show_vdd2");
        if (value.empty()) return true;
        convertToUpper(value);
        return value == "TRUE";
    }


    bool getCurrentShowFullRes() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "show_full_res");
        if (value.empty()) return true; // Default: true for mini, false for micro
        convertToUpper(value);
        return value != "FALSE";
    }

    bool getCurrentShowSOCVoltage() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "show_soc_voltage");
        if (value.empty()) return false; // Default: false for mini, true for micro
        convertToUpper(value);
        return value != "FALSE";
    }

    bool getCurrentShowpartLoadCPUGPU() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "show_RAM_load_CPU_GPU");
        if (value.empty()) return false; // Default: false for mini, true for micro
        convertToUpper(value);
        return value != "FALSE";
    }

    std::string getCurrentRamInfoMode(const std::string& section) {
        const std::string value = ult::parseValueFromIniSection(configIniPath, section, "ram_info_mode");
        return (value == "Bandwidth") ? "Bandwidth" : "Load";
    }

    bool getCurrentInvertBatteryDisplay() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "invert_battery_display");
        if (value.empty()) return isMiniMode ? true : false; // Default: false for mini, true for micro
        convertToUpper(value);
        return value != "FALSE";
    }

    bool getCurrentUseDTCSymbol() {
        const std::string section = isMiniMode ? "mini" : "micro";
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "use_dtc_symbol");
        if (value.empty()) return true;
        convertToUpper(value);
        return value == "TRUE";
    }

    bool getCurrentUseDynamicColors() {
        const std::string section = isFPSGraphMode? "fps-graph" : (isMiniMode ? "mini" : "micro");
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "use_dynamic_colors");
        if (value.empty()) return true;
        convertToUpper(value);
        return value == "TRUE";
    }

    bool getCurrentUseIntegerCounter(const std::string& section) {
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "use_integer_counter");
        if (value.empty()) return false;  // Default is false (screenshots enabled)
        convertToUpper(value);
        return value != "FALSE";  // True if not explicitly "FALSE"
    }

    bool getCurrentDisableScreenshots(const std::string& section) {
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "disable_screenshots");
        if (value.empty()) return false;  // Default is false (screenshots enabled)
        convertToUpper(value);
        return value != "FALSE";  // True if not explicitly "FALSE"
    }

    bool getCurrentSleepExit(const std::string& section) {
        std::string value = ult::parseValueFromIniSection(configIniPath, section, "sleep_exit");
        if (value.empty()) return false;
        convertToUpper(value);
        return value != "FALSE";  // True if not explicitly "FALSE"
    }

    // Full mode toggle helpers
    bool getCurrentShowRealFreqs() {
        std::string value = ult::parseValueFromIniSection(configIniPath, "full", "show_real_freqs");
        if (value.empty()) return true;
        convertToUpper(value);
        return value != "FALSE";
    }

	bool getCurrentFullRealTemps() {
		std::string value = ult::parseValueFromIniSection(configIniPath, "full", "real_temps");
		if (value.empty()) return false;
		convertToUpper(value);
		return value == "TRUE";
	}

    bool getCurrentShowDeltas() {
        std::string value = ult::parseValueFromIniSection(configIniPath, "full", "show_deltas");
        if (value.empty()) return true;
        convertToUpper(value);
        return value != "FALSE";
    }

    bool getCurrentShowTargetFreqs() {
        std::string value = ult::parseValueFromIniSection(configIniPath, "full", "show_target_freqs");
        if (value.empty()) return true;
        convertToUpper(value);
        return value != "FALSE";
    }

    bool getCurrentShowFPS() {
        std::string value = ult::parseValueFromIniSection(configIniPath, "full", "show_fps");
        if (value.empty()) return true;
        convertToUpper(value);
        return value != "FALSE";
    }

    bool getCurrentShowRES() {
        std::string value = ult::parseValueFromIniSection(configIniPath, "full", "show_res");
        if (value.empty()) return true;
        convertToUpper(value);
        return value != "FALSE";
    }

    bool getCurrentShowRDSD() {
        std::string value = ult::parseValueFromIniSection(configIniPath, "full", "show_read_speed");
        if (value.empty()) return true;
        convertToUpper(value);
        return value != "FALSE";
    }
};

// Refresh Rate Configuration
class RefreshRateConfig : public tsl::Gui {
private:
    std::string modeName;
    bool isMiniMode;
    bool isMicroMode;
    bool isFullMode;
    bool isGameResolutionsMode;
    bool isFPSCounterMode;
    bool isFPSGraphMode;
    int currentRate;

public:
    RefreshRateConfig(const std::string& mode) : modeName(mode) {
        isMiniMode = (mode == "Mini");
        isMicroMode = (mode == "Micro");
        isFullMode = (mode == "Full");
        isGameResolutionsMode = (mode == "Game Resolutions");
        isFPSCounterMode = (mode == "FPS Counter");
        isFPSGraphMode = (mode == "FPS Graph");

        std::string section;
        if (isMiniMode) section = "mini";
        else if (isMicroMode) section = "micro";
        else if (isFullMode) section = "full";
        else if (isGameResolutionsMode) section = "game_resolutions";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";

        const std::string value = ult::parseValueFromIniSection(configIniPath, section, "refresh_rate");
        int defaultRate = (isGameResolutionsMode) ? 10 : ((isFPSCounterMode || isFPSGraphMode) ? 30 : 1);
        currentRate = value.empty() ? defaultRate : std::clamp(atoi(value.c_str()), 1, 60);
    }

    ~RefreshRateConfig() {
        lastSelectedListItem = nullptr;
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("Refresh Rate"));

        static const std::vector<int> rates = {1, 2, 3, 5, 10, 15, 30, 60};
        for (int rate : rates) {
            auto* rateItem = new tsl::elm::ListItem(std::to_string(rate) + " Hz");
            if (rate == currentRate) {
                rateItem->setValue(ult::CHECKMARK_SYMBOL);
                lastSelectedListItem = rateItem;
            }
            rateItem->setClickListener([this, rateItem, rate](uint64_t keys) {
                if (keys & KEY_A) {
                    std::string section;
                    if (isMiniMode) section = "mini";
                    else if (isMicroMode) section = "micro";
                    else if (isFullMode) section = "full";
                    else if (isGameResolutionsMode) section = "game_resolutions";
                    else if (isFPSCounterMode) section = "fps-counter";
                    else if (isFPSGraphMode) section = "fps-graph";

                    ult::setIniFileValue(configIniPath, section, "refresh_rate", std::to_string(rate));
                    rateItem->setValue(ult::CHECKMARK_SYMBOL);
                    if (lastSelectedListItem && rateItem != lastSelectedListItem)
                        lastSelectedListItem->setValue("");
                    lastSelectedListItem = rateItem;
                    return true;
                }
                return false;
            });
            list->addItem(rateItem);
        }

        list->jumpToItem("", ult::CHECKMARK_SYMBOL, false);

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Configuration");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            jumpItemName = "Refresh Rate";
            jumpItemValue = "";
            jumpItemExactMatch = false;

            tsl::swapTo<ConfiguratorOverlay>(SwapDepth(2), modeName);
            return true;
        }
        return false;
    }
};

// Frame Padding Configuration (Mini only)
class FramePaddingConfig : public tsl::Gui {
private:
    std::string modeName;
    int currentPadding;

public:
    FramePaddingConfig(const std::string& mode) : modeName(mode) {
        const std::string value = ult::parseValueFromIniSection(configIniPath, "mini", "frame_padding");
        currentPadding = value.empty() ? 10 : std::clamp(atoi(value.c_str()), 0, 14); // max value 14
    }

    ~FramePaddingConfig() {
        lastSelectedListItem = nullptr;
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("Frame Padding"));

        static const std::vector<int> paddingValues = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
        for (int padding : paddingValues) {
            auto* paddingItem = new tsl::elm::ListItem(std::to_string(padding) + " px");
            if (padding == currentPadding) {
                paddingItem->setValue(ult::CHECKMARK_SYMBOL);
                lastSelectedListItem = paddingItem;
            }
            paddingItem->setClickListener([this, paddingItem, padding](uint64_t keys) {
                if (keys & KEY_A) {
                    ult::setIniFileValue(configIniPath, "mini", "frame_padding", std::to_string(padding));
                    paddingItem->setValue(ult::CHECKMARK_SYMBOL);
                    if (lastSelectedListItem && paddingItem != lastSelectedListItem)
                        lastSelectedListItem->setValue("");
                    lastSelectedListItem = paddingItem;
                    return true;
                }
                return false;
            });
            list->addItem(paddingItem);
        }

        list->jumpToItem("", ult::CHECKMARK_SYMBOL, false);

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Configuration");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            jumpItemName = "Frame Padding";
            jumpItemValue = "";
            jumpItemExactMatch = false;

            tsl::swapTo<ConfiguratorOverlay>(SwapDepth(2), modeName);
            return true;
        }
        return false;
    }
};


// Font Size Selector
class FontSizeSelector : public tsl::Gui {
private:
    std::string modeName;
    std::string fontType;
    bool isMiniMode;
    bool isMicroMode;
    bool isFPSCounterMode;
    std::string title;

public:
    FontSizeSelector(const std::string& mode, const std::string& type)
        : modeName(mode), fontType(type) {
            isMiniMode = (mode == "Mini");
            isMicroMode = (mode == "Micro");
            isFPSCounterMode = (mode == "FPS Counter");
            title = fontType;
            title[0] = std::toupper(title[0]);
            title += " Font Size";
        }
    ~FontSizeSelector() {
        lastSelectedListItem = nullptr;
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader(title));

        std::string section;
        if (isMiniMode) section = "mini";
        else if (isMicroMode) section = "micro";
        else if (isFPSCounterMode) section = "fps-counter";

        const std::string keyName = fontType + "_font_size";
        const std::string currentValue = ult::parseValueFromIniSection(configIniPath, section, keyName);
        int defaultSize = isFPSCounterMode ? 40 : 15;
        const int currentSize = currentValue.empty() ? defaultSize : atoi(currentValue.c_str());

        // Font size range depends on mode
        int minSize = 8;
        int maxSize;
        if (isFPSCounterMode) maxSize = 150;
        else if (isMiniMode) maxSize = 22;
        else maxSize = 18; // Micro mode

        for (int size = minSize; size <= maxSize; size++) {
            auto* sizeItem = new tsl::elm::ListItem(std::to_string(size) + " pt");
            if (size == currentSize) {
                sizeItem->setValue(ult::CHECKMARK_SYMBOL);
                lastSelectedListItem = sizeItem;
            }
            sizeItem->setClickListener([this, sizeItem, size, keyName, section](uint64_t keys) {
                if (keys & KEY_A) {
                    ult::setIniFileValue(configIniPath, section, keyName, std::to_string(size));
                    sizeItem->setValue(ult::CHECKMARK_SYMBOL);
                    if (lastSelectedListItem && lastSelectedListItem != sizeItem)
                        lastSelectedListItem->setValue("");
                    lastSelectedListItem = sizeItem;
                    return true;
                }
                return false;
            });
            list->addItem(sizeItem);
        }

        list->jumpToItem("", ult::CHECKMARK_SYMBOL, false);

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Font Sizes");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            jumpItemName = title;
            jumpItemValue = "";
            jumpItemExactMatch = false;

            tsl::swapTo<FontSizeConfig>(SwapDepth(2), modeName);
            return true;
        }
        return false;
    }
};

// Font Size Configuration
class FontSizeConfig : public tsl::Gui {
private:
    std::string modeName;
    bool isMiniMode;
    bool isMicroMode;
    bool isFPSCounterMode;

public:
    FontSizeConfig(const std::string& mode) : modeName(mode) {
        isMiniMode = (mode == "Mini");
        isMicroMode = (mode == "Micro");
        isFPSCounterMode = (mode == "FPS Counter");
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("Font Sizes"));

        std::string section;
        if (isMiniMode) section = "mini";
        else if (isMicroMode) section = "micro";
        else if (isFPSCounterMode) section = "fps-counter";

        const std::string handheldValue = ult::parseValueFromIniSection(configIniPath, section, "handheld_font_size");
        const std::string dockedValue = ult::parseValueFromIniSection(configIniPath, section, "docked_font_size");

        int defaultSize = isFPSCounterMode ? 40 : 15;
        const int handheldSize = handheldValue.empty() ? defaultSize : atoi(handheldValue.c_str());
        const int dockedSize = dockedValue.empty() ? defaultSize : atoi(dockedValue.c_str());

        auto* handheldItem = new tsl::elm::ListItem("Handheld Font Size");
        handheldItem->setValue(std::to_string(handheldSize) + " pt");
        handheldItem->setClickListener([this](uint64_t keys) {
            if (keys & KEY_A) {
                tsl::changeTo<FontSizeSelector>(modeName, "handheld");
                return true;
            }
            return false;
        });
        list->addItem(handheldItem);

        auto* dockedItem = new tsl::elm::ListItem("Docked Font Size");
        dockedItem->setValue(std::to_string(dockedSize) + " pt");
        dockedItem->setClickListener([this](uint64_t keys) {
            if (keys & KEY_A) {
                tsl::changeTo<FontSizeSelector>(modeName, "docked");
                return true;
            }
            return false;
        });
        list->addItem(dockedItem);

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Configuration");
        rootFrame->setContent(list);
        list->jumpToItem(jumpItemName, jumpItemValue, jumpItemExactMatch);
        {
            jumpItemName = "";
            jumpItemValue = "";
            jumpItemExactMatch = false;
        }
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            tsl::goBack();
            return true;
        }
        return false;
    }
};

// Color Selector
class ColorSelector : public tsl::Gui {
private:
    std::string modeName;
    std::string modeTitle;
    std::string colorKey;
    std::string defaultValue;
    bool isMiniMode;
    bool isMicroMode;
    bool isFullMode;
    bool isGameResolutionsMode;
    bool isFPSCounterMode;
    bool isFPSGraphMode;
    bool isBackgroundColor;
    bool isTextBasedColor;

public:
    ColorSelector(const std::string& mode, const std::string& title, const std::string& key, const std::string& def)
        : modeName(mode), modeTitle(title), colorKey(key), defaultValue(def) {
            isMiniMode = (mode == "Mini");
            isMicroMode = (mode == "Micro");
            isFullMode = (mode == "Full");
            isGameResolutionsMode = (mode == "Game Resolutions");
            isFPSCounterMode = (mode == "FPS Counter");
            isFPSGraphMode = (mode == "FPS Graph");

            // Determine if this is a background color or text-based color
            isBackgroundColor = (key == "background_color" || key == "focus_background_color" ||
                               (isFPSGraphMode && (key == "fps_counter_color" || key == "dashed_line_color")));

            isTextBasedColor = (key == "text_color" || key == "separator_color" || key == "cat_color" ||
                              (isFPSGraphMode && (key == "border_color" || key == "max_fps_text_color" ||
                               key == "min_fps_text_color" || key == "main_line_color" ||
                               key == "rounded_line_color" || key == "perfect_line_color")));
        }

    ~ColorSelector() {
        lastSelectedListItem = nullptr;
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader(modeTitle));

        std::string section;
        if (isMiniMode) section = "mini";
        else if (isMicroMode) section = "micro";
        else if (isFullMode) section = "full";
        else if (isGameResolutionsMode) section = "game_resolutions";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";

        std::string currentValue = ult::parseValueFromIniSection(configIniPath, section, colorKey);
        if (currentValue.empty()) currentValue = defaultValue;

        // Extract the color without alpha for comparison (for backgrounds and text colors)
        std::string currentColorWithoutAlpha = extractColorWithoutAlpha(currentValue);

        // Updated colors list with comprehensive color palette
        static const std::vector<std::pair<std::string, std::string>> colors = {
            // Grays & Basics
            {"Black", "#000F"},
            {"Dark Gray", "#333F"},
            {"Gray", "#444F"},
            {"Light Gray", "#888F"},
            {"Silver", "#CCCF"},
            {"White", "#FFFF"},

            // Reds
            {"Dark Red", "#800F"},
            {"Red", "#F00F"},
            {"Light Red", "#F88F"},
            {"Pink", "#F8AF"},

            // Greens
            {"Dark Green", "#080F"},
            {"Green", "#0F0F"},
            {"Lime Green", "#0C0F"},
            {"Light Green", "#8F8F"},

            // Blues
            {"Dark Blue", "#003F"},
            {"Blue", "#00FF"},
            {"Light Blue", "#2DFF"},
            {"Sky Blue", "#8CFF"},

            // Purples
            {"Dark Purple", "#808F"},
            {"Purple", "#80FF"},
            {"Light Purple", "#C8FF"},
            {"Violet", "#A0FF"},

            // Yellows & Oranges
            {"Orange", "#F80F"},
            {"Yellow", "#FF0F"},
            {"Light Yellow", "#FFCF"},

            // Cyans & Teals
            {"Teal", "#088F"},
            {"Cyan", "#0FFF"},
            {"Light Cyan", "#8FFF"},

            // Magentas & Pinks
            {"Magenta", "#F0FF"},
            {"Hot Pink", "#F8CF"},

            // Browns
            {"Brown", "#840F"},
            {"Light Brown", "#A86F"}
        };

        std::string _jumpItemValue;
        for (const auto& color : colors) {
            auto* colorItem = new tsl::elm::ListItem(color.first);

            // For display, show the color code based on type
            std::string displayValue;
            if (isTextBasedColor || isBackgroundColor) {
                // For ALL text-based colors AND background colors, show without the alpha
                displayValue = extractColorWithoutAlpha(color.second);
            } else {
                // For any remaining FPS Graph colors (shouldn't happen now), keep original behavior
                displayValue = color.second;
            }

            colorItem->setValue(displayValue);

            // Check if this is the selected color
            bool isSelected = false;
            if (isBackgroundColor || isTextBasedColor) {
                // For background and text colors, compare without alpha
                isSelected = (extractColorWithoutAlpha(color.second) == currentColorWithoutAlpha);
            } else {
                // For any remaining FPS Graph colors (shouldn't happen now)
                isSelected = (color.second == currentValue);
            }

            if (isSelected) {
                colorItem->setValue(displayValue + " " + ult::CHECKMARK_SYMBOL);
                lastSelectedListItem = colorItem;
                _jumpItemValue = displayValue + " " + ult::CHECKMARK_SYMBOL;
            }

            colorItem->setClickListener([this, colorItem, color, section, displayValue](uint64_t keys) {
                if (keys & KEY_A) {
                    std::string valueToSave = color.second;

                    if (isBackgroundColor) {
                        // For background colors, preserve existing alpha
                        std::string existingColor = ult::parseValueFromIniSection(configIniPath, section, colorKey);
                        if (!existingColor.empty() && existingColor.length() == 5) {
                            char existingAlpha = existingColor[4];
                            valueToSave = setAlphaInColor(color.second, existingAlpha);
                        }
                    } else if (isTextBasedColor) {
                        // For text-based colors, ensure alpha is always F
                        valueToSave = setAlphaInColor(color.second, 'F');
                    }
                    // For any remaining FPS Graph colors (shouldn't happen now), use as-is

                    ult::setIniFileValue(configIniPath, section, colorKey, valueToSave);

                    // Update the UI - clear old checkmark and set new one
                    if (lastSelectedListItem && lastSelectedListItem != colorItem) {
                        // Get the display value for the old selected item
                        std::string oldDisplayValue;
                        for (const auto& c : colors) {
                            if (lastSelectedListItem->getText() == c.first) {
                                if (isTextBasedColor || isBackgroundColor) {
                                    oldDisplayValue = extractColorWithoutAlpha(c.second);
                                } else {
                                    oldDisplayValue = c.second;
                                }
                                break;
                            }
                        }
                        lastSelectedListItem->setValue(oldDisplayValue);
                    }

                    // Set new checkmark
                    colorItem->setValue(displayValue + " " + ult::CHECKMARK_SYMBOL);
                    lastSelectedListItem = colorItem;
                    return true;
                }
                return false;
            });
            list->addItem(colorItem);
        }
        list->jumpToItem("", _jumpItemValue, false);

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Colors");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            jumpItemName = modeTitle;
            jumpItemValue = "";
            jumpItemExactMatch = false;

            tsl::swapTo<ColorConfig>(SwapDepth(2), modeName);
            return true;
        }
        return false;
    }
};

// Color Configuration
class ColorConfig : public tsl::Gui {
private:
    std::string modeName;
    bool isMiniMode;
    bool isMicroMode;
    bool isFullMode;
    bool isGameResolutionsMode;
    bool isFPSCounterMode;
    bool isFPSGraphMode;

public:
    ColorConfig(const std::string& mode) : modeName(mode) {
        isMiniMode = (mode == "Mini");
        isMicroMode = (mode == "Micro");
        isFullMode = (mode == "Full");
        isGameResolutionsMode = (mode == "Game Resolutions");
        isFPSCounterMode = (mode == "FPS Counter");
        isFPSGraphMode = (mode == "FPS Graph");

        // Full mode should never access color configuration
        //if (isFullMode) {
        //    // This should not happen, but if it does, go back
        //    tsl::goBack();
        //}
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("Colors"));

        auto getCurrentColor = [this](const std::string& key, const std::string& def) {
            std::string section;
            if (isMiniMode) section = "mini";
            else if (isMicroMode) section = "micro";
            else if (isFullMode) section = "full";
            else if (isGameResolutionsMode) section = "game_resolutions";
            else if (isFPSCounterMode) section = "fps-counter";
            else if (isFPSGraphMode) section = "fps-graph";

            std::string value = ult::parseValueFromIniSection(configIniPath, section, key);
            return value.empty() ? def : value;
        };

        auto getColorName = [](const std::string& hexColor) -> std::string {
            // Extract RGB without alpha for comparison
            std::string rgb = hexColor;
            if (hexColor.length() == 5 && hexColor[0] == '#') {
                rgb = hexColor.substr(0, 4);
            }

            // Map of hex colors to names (RGB only, no alpha)
            static const std::map<std::string, std::string> colorNames = {
                // Grays & Basics
                {"#000", "Black"},
                {"#333", "Dark Gray"},
                {"#444", "Gray"},
                {"#888", "Light Gray"},
                {"#CCC", "Silver"},
                {"#FFF", "White"},

                // Reds
                {"#800", "Dark Red"},
                {"#F00", "Red"},
                {"#F88", "Light Red"},
                {"#F8A", "Pink"},

                // Greens
                {"#080", "Dark Green"},
                {"#0F0", "Green"},
                {"#0C0", "Lime Green"},
                {"#8F8", "Light Green"},

                // Blues
                {"#003", "Dark Blue"},
                {"#00F", "Blue"},
                {"#2DF", "Light Blue"},
                {"#8CF", "Sky Blue"},

                // Purples
                {"#808", "Dark Purple"},
                {"#80F", "Purple"},
                {"#C8F", "Light Purple"},
                {"#A0F", "Violet"},

                // Yellows & Oranges
                {"#F80", "Orange"},
                {"#FF0", "Yellow"},
                {"#FFC", "Light Yellow"},

                // Cyans & Teals
                {"#088", "Teal"},
                {"#0FF", "Cyan"},
                {"#8FF", "Light Cyan"},

                // Magentas & Pinks
                {"#F0F", "Magenta"},
                {"#F8C", "Hot Pink"},

                // Browns
                {"#840", "Brown"},
                {"#A86", "Light Brown"}
            };

            auto it = colorNames.find(rgb);
            if (it != colorNames.end()) {
                // Special case for black/transparent disambiguation
                if (rgb == "#000" && hexColor.length() == 5) {
                    char alpha = hexColor[4];
                    if (alpha == '0') return "Transparent";
                    else return "Black";
                }
                return it->second;
            }
            return rgb; // Return hex if no name found
        };

        auto getAlphaPercentage = [](const std::string& color) -> std::string {
            if (color.length() == 5 && color[0] == '#') {
                char alpha = color[4];
                switch(alpha) {
                    case '0': return "0%";
                    case '1': return "10%";
                    case '3': return "20%";
                    case '4': return "30%";
                    case '6': return "40%";
                    case '8': return "50%";
                    case '9': return "60%";
                    case 'B': case 'b': return "70%";
                    case 'C': case 'c': return "80%";
                    case 'E': case 'e': return "90%";
                    case 'F': case 'f': return "100%";
                    default: return "60%";
                }
            }
            return "60%";
        };

        if (!isFullMode) {
            // Background Color (all modes)
            auto* bgColor = new tsl::elm::ListItem("Background Color");
            std::string bgDefault = "#0009";
            std::string bgCurrentColor = getCurrentColor("background_color", bgDefault);
            // Display color name instead of hex
            bgColor->setValue(getColorName(bgCurrentColor));
            bgColor->setClickListener([this, bgDefault](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Background Color", "background_color", bgDefault);
                    return true;
                }
                return false;
            });
            list->addItem(bgColor);

            // Background Alpha (new)
            auto* bgAlpha = new tsl::elm::ListItem("Background Alpha");
            bgAlpha->setValue(getAlphaPercentage(bgCurrentColor));
            bgAlpha->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<AlphaSelector>(modeName, "background_color", "Background Alpha");
                    return true;
                }
                return false;
            });
            list->addItem(bgAlpha);

            if (isMiniMode || isFPSCounterMode || isFPSGraphMode || isGameResolutionsMode) {
                // Mini mode: has focus background
                auto* focusBgColor = new tsl::elm::ListItem("Focus Color");
                std::string focusCurrentColor = getCurrentColor("focus_background_color", "#000F");
                focusBgColor->setValue(getColorName(focusCurrentColor));
                focusBgColor->setClickListener([this](uint64_t keys) {
                    if (keys & KEY_A) {
                        tsl::changeTo<ColorSelector>(modeName, "Focus Color", "focus_background_color", "#000F");
                        return true;
                    }
                    return false;
                });
                list->addItem(focusBgColor);

                // Focus Alpha (new)
                auto* focusAlpha = new tsl::elm::ListItem("Focus Alpha");
                focusAlpha->setValue(getAlphaPercentage(focusCurrentColor));
                focusAlpha->setClickListener([this](uint64_t keys) {
                    if (keys & KEY_A) {
                        tsl::changeTo<AlphaSelector>(modeName, "focus_background_color", "Focus Alpha");
                        return true;
                    }
                    return false;
                });
                list->addItem(focusAlpha);
            }
        }

        // Text Color (all modes)
        auto* textColor = new tsl::elm::ListItem("Text Color");
        std::string textCurrentColor = getCurrentColor("text_color", "#FFFF");
        // Display color name for text colors
        textColor->setValue(getColorName(textCurrentColor));
        textColor->setClickListener([this](uint64_t keys) {
            if (keys & KEY_A) {
                tsl::changeTo<ColorSelector>(modeName, "Text Color", "text_color", "#FFFF");
                return true;
            }
            return false;
        });
        list->addItem(textColor);

        if (isFPSGraphMode) {
            // FPS Graph specific colors
            struct ColorSetting {
                std::string name;
                std::string key;
                std::string defaultVal;
                bool isBackgroundType;  // true for colors that allow alpha adjustment
            };

            // Game Resolutions: only category color (no separator)
            auto* catColor = new tsl::elm::ListItem("Category Color");
            catColor->setValue(getColorName(getCurrentColor("cat_color", "#0F0F")));
            catColor->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Category Color", "cat_color", "#0F0F");
                    return true;
                }
                return false;
            });
            list->addItem(catColor);

            static const std::vector<ColorSetting> fpsGraphColors = {
                {"FPS Counter", "fps_counter_color", "#888C", true},      // background type
                {"Border", "border_color", "#2DFF", false},               // text type
                {"Dashed Line", "dashed_line_color", "#8888", true},      // background type
                {"Max FPS Text", "max_fps_text_color", "#FFFF", false},   // text type
                {"Min FPS Text", "min_fps_text_color", "#FFFF", false},   // text type
                {"Main Line", "main_line_color", "#FFFF", false},         // text type
                {"Rounded Line", "rounded_line_color", "#F0FF", false},   // text type
                {"Perfect Line", "perfect_line_color", "#0C0F", false}    // text type
            };

            for (const auto& color : fpsGraphColors) {
                auto* colorItem = new tsl::elm::ListItem(color.name + " Color");
                const std::string currentVal = getCurrentColor(color.key, color.defaultVal);

                if (color.isBackgroundType) {
                    // For background-type colors, show color name
                    colorItem->setValue(getColorName(currentVal));
                } else {
                    // For text-type colors, show color name
                    colorItem->setValue(getColorName(currentVal));
                }

                colorItem->setClickListener([this, color](uint64_t keys) {
                    if (keys & KEY_A) {
                        tsl::changeTo<ColorSelector>(modeName, color.name, color.key, color.defaultVal);
                        return true;
                    }
                    return false;
                });
                list->addItem(colorItem);

                // Add alpha selector for background-type colors
                if (color.isBackgroundType) {
                    auto* alphaItem = new tsl::elm::ListItem(color.name + " Alpha");
                    alphaItem->setValue(getAlphaPercentage(currentVal));
                    alphaItem->setClickListener([this, color](uint64_t keys) {
                        if (keys & KEY_A) {
                            tsl::changeTo<AlphaSelector>(modeName, color.key, color.name + " Alpha");
                            return true;
                        }
                        return false;
                    });
                    list->addItem(alphaItem);
                }
            }

        } else if (isFullMode) {
            auto* catColor1 = new tsl::elm::ListItem("Category Color 1");
            // Display color name for category colors
            catColor1->setValue(getColorName(getCurrentColor("cat_color_1", "#8FFF")));
            catColor1->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Category Color 1", "cat_color_1", "#8FFF");
                    return true;
                }
                return false;
            });
            list->addItem(catColor1);

            auto* catColor2 = new tsl::elm::ListItem("Category Color 2");
            // Display color name for category colors
            catColor2->setValue(getColorName(getCurrentColor("cat_color_2", "#2DFF")));
            catColor2->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Category Color 2", "cat_color_2", "#2DFF");
                    return true;
                }
                return false;
            });
            list->addItem(catColor2);

            auto* sepColor = new tsl::elm::ListItem("Separator Color");
            // Display color name for separator colors
            sepColor->setValue(getColorName(getCurrentColor("separator_color", "#888F")));
            sepColor->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Separator Color", "separator_color", "#888F");
                    return true;
                }
                return false;
            });
            list->addItem(sepColor);
        } else if (isMiniMode) {
            auto* catColor = new tsl::elm::ListItem("Category Color");
            // Display color name for category colors
            catColor->setValue(getColorName(getCurrentColor("cat_color", "#2DFF")));
            catColor->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Category Color", "cat_color", "#2DFF");
                    return true;
                }
                return false;
            });
            list->addItem(catColor);

            auto* sepColor = new tsl::elm::ListItem("Separator Color");
            // Display color name for separator colors
            sepColor->setValue(getColorName(getCurrentColor("separator_color", "#888F")));
            sepColor->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Separator Color", "separator_color", "#888F");
                    return true;
                }
                return false;
            });
            list->addItem(sepColor);

        } else if (isMicroMode) {
            auto* catColor = new tsl::elm::ListItem("Category Color");
            catColor->setValue(getColorName(getCurrentColor("cat_color", "#2DFF")));
            catColor->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Category Color", "cat_color", "#2DFF");
                    return true;
                }
                return false;
            });
            list->addItem(catColor);

            // Micro mode: separator and category colors (no focus background like Mini)
            auto* sepColor = new tsl::elm::ListItem("Separator Color");
            sepColor->setValue(getColorName(getCurrentColor("separator_color", "#888F")));
            sepColor->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Separator Color", "separator_color", "#888F");
                    return true;
                }
                return false;
            });
            list->addItem(sepColor);


        } else if (isGameResolutionsMode) {
            // Game Resolutions: only category color (no separator)
            auto* catColor = new tsl::elm::ListItem("Category Color");
            catColor->setValue(getColorName(getCurrentColor("cat_color", "#2DFF")));
            catColor->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ColorSelector>(modeName, "Category Color", "cat_color", "#2DFF");
                    return true;
                }
                return false;
            });
            list->addItem(catColor);
        }
        // FPS Counter mode: only background and text colors (already added above)
        // Full mode: NO color settings at all (excluded from this function)

        list->jumpToItem(jumpItemName, jumpItemValue, jumpItemExactMatch);
        {
            jumpItemName = "";
            jumpItemValue = "";
            jumpItemExactMatch = false;
        }

        //list->disableCaching();
        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Configuration");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            tsl::goBack();
            return true;
        }
        return false;
    }
};

// Show Configuration (Mini/Micro only)
class ShowConfig : public tsl::Gui {
private:
    std::string modeName;
    bool isMiniMode;
    bool isMicroMode;
    std::vector<std::string> elementOrder;
    std::unordered_set<std::string> enabledElements;

public:
    ShowConfig(const std::string& mode) : modeName(mode) {
        isMiniMode = (mode == "Mini");
        isMicroMode = (mode == "Micro");
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("Elements " + ult::DIVIDER_SYMBOL + " \uE0E3 Move Down " + ult::DIVIDER_SYMBOL + " \uE0E2 Move Up"));

        const std::string section = isMiniMode ? "mini" : "micro";
        std::string showValue = ult::parseValueFromIniSection(configIniPath, section, "show");
        std::string orderValue = ult::parseValueFromIniSection(configIniPath, section, "element_order");

        if (showValue.empty()) {
            showValue = isMiniMode ? "DTC+BAT+CPU+GPU+RAM+TMP+FPS+RES" : "FPS+CPU+GPU+RAM+SOC+BAT+DTC";
        }
        convertToUpper(showValue);

        enabledElements.clear();
        ult::StringStream ss(showValue);
        std::string item;
        while (ss.getline(item, '+')) {
            if (!item.empty()) {
                enabledElements.insert(item);
            }
        }

        elementOrder.clear();
        if (!orderValue.empty()) {
            convertToUpper(orderValue);
            ult::StringStream orderSS(orderValue);
            std::string orderItem;
            while (orderSS.getline(orderItem, '+')) {
                if (!orderItem.empty()) {
                    elementOrder.push_back(orderItem);
                }
            }
        } else {
            ult::StringStream ss2(showValue);
            while (ss2.getline(item, '+')) {
                if (!item.empty()) {
                    elementOrder.push_back(item);
                }
            }
        }

        static constexpr std::string_view miniElements[] = {
            "DTC","BAT","CPU","GPU","RAM","MEM","READ","SOC","TMP","FPS","RES"
        };

        static constexpr std::string_view microElements[] = {
            "FPS","CPU","GPU","RAM","READ","SOC","TMP","RES","BAT","DTC"
        };

        // Use span or array reference instead of pointer
        const auto* allElements = isMiniMode ? miniElements : microElements;
        const size_t allElementsSize = isMiniMode ? std::size(miniElements) : std::size(microElements);

        elementOrder.clear();
        if (!orderValue.empty()) {
            convertToUpper(orderValue);
            ult::StringStream orderSS(orderValue);
            std::string orderItem;
            while (orderSS.getline(orderItem, '+')) {
                if (!orderItem.empty()) {
                    elementOrder.push_back(orderItem);
                }
            }
        } else {
            // Initialize with allElements order instead
            for (size_t i = 0; i < allElementsSize; i++) {
                auto elem = allElements[i];
                if (!isMiniMode && elem == "MEM")
                    continue;
                elementOrder.emplace_back(elem);
            }
        }

        for (size_t i = 0; i < elementOrder.size(); i++) {
            const std::string& element = elementOrder[i];
            const bool isEnabled = enabledElements.find(element) != enabledElements.end();

            auto* elementItem = new tsl::elm::ListItem(element);
            elementItem->setValue(isEnabled ? ult::ON : ult::OFF, !isEnabled);

            elementItem->setClickListener([this, elementItem, element](uint64_t keys) {
                static bool hasNotTriggeredAnimation = false;

                if (hasNotTriggeredAnimation) {
                    elementItem->triggerClickAnimation();
                    hasNotTriggeredAnimation = false;
                }

                if (keys & KEY_A) {
                    const bool currentlyEnabled = enabledElements.find(element) != enabledElements.end();

                    if (currentlyEnabled) {
                        enabledElements.erase(element);
                    } else {
                        enabledElements.insert(element);
                    }

                    updateShowAndOrder();
                    jumpItemName = element;
                    jumpItemValue = "";
                    jumpItemExactMatch = true;
                    hasNotTriggeredAnimation = true;

                    tsl::swapTo<ShowConfig>(SwapDepth(1), modeName);
                    return true;
                }
                else if (keys & KEY_Y || keys & KEY_X) {
                    size_t currentPos = 0;
                    for (size_t j = 0; j < elementOrder.size(); j++) {
                        if (elementOrder[j] == element) {
                            currentPos = j;
                            break;
                        }
                    }

                    if (keys & KEY_X) {
                        if (currentPos > 0) {
                            std::swap(elementOrder[currentPos], elementOrder[currentPos - 1]);
                        } else {
                            const std::string temp = elementOrder[0];
                            for (size_t j = 0; j < elementOrder.size() - 1; j++) {
                                elementOrder[j] = elementOrder[j + 1];
                            }
                            elementOrder[elementOrder.size() - 1] = temp;
                        }
                        triggerRumbleClick.store(true, std::memory_order_release);
                        triggerMoveSound.store(true, std::memory_order_release);
                    } else if (keys & KEY_Y) {
                        if (currentPos < elementOrder.size() - 1) {
                            std::swap(elementOrder[currentPos], elementOrder[currentPos + 1]);
                        } else {
                            const std::string temp = elementOrder[elementOrder.size() - 1];
                            for (size_t j = elementOrder.size() - 1; j > 0; j--) {
                                elementOrder[j] = elementOrder[j - 1];
                            }
                            elementOrder[0] = temp;
                        }
                        triggerRumbleClick.store(true, std::memory_order_release);
                        triggerMoveSound.store(true, std::memory_order_release);
                    }

                    updateShowAndOrder();
                    jumpItemName = element;
                    jumpItemValue = "";
                    jumpItemExactMatch = true;

                    tsl::swapTo<ShowConfig>(SwapDepth(1), modeName);
                    return true;
                }
                return false;
            });

            list->addItem(elementItem);
        }

        list->jumpToItem(jumpItemName, jumpItemValue, jumpItemExactMatch);
        {
            jumpItemName = "";
            jumpItemValue = "";
            jumpItemExactMatch = false;
        }

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", "Configuration");
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            tsl::goBack();
            return true;
        }
        return false;
    }

private:
    void updateShowAndOrder() {
        std::string newShowValue;
        std::string newOrderValue;
        bool showFirst = true;
        bool orderFirst = true;

        for (const std::string& element : elementOrder) {
            if (!orderFirst) {
                newOrderValue += "+";
            }
            newOrderValue += element;
            orderFirst = false;

            if (enabledElements.find(element) != enabledElements.end()) {
                if (!showFirst) {
                    newShowValue += "+";
                }
                newShowValue += element;
                showFirst = false;
            }
        }

        const std::string section = isMiniMode ? "mini" : "micro";
        ult::setIniFileValue(configIniPath, section, "show", newShowValue);
        ult::setIniFileValue(configIniPath, section, "element_order", newOrderValue);
    }
};

// Main Configurator
class ConfiguratorOverlay : public tsl::Gui {
private:
    std::string modeName;
    bool isMiniMode;
    bool isMicroMode;
    bool isFullMode;
    bool isGameResolutionsMode;
    bool isFPSCounterMode;
    bool isFPSGraphMode;

public:
    ConfiguratorOverlay(const std::string& mode) : modeName(mode) {
        isMiniMode = (mode == "Mini");
        isMicroMode = (mode == "Micro");
        isFullMode = (mode == "Full");
        isGameResolutionsMode = (mode == "Game Resolutions");
        isFPSCounterMode = (mode == "FPS Counter");
        isFPSGraphMode = (mode == "FPS Graph");
    }

    virtual tsl::elm::Element* createUI() override {
        auto* list = new tsl::elm::List();
        list->addItem(new tsl::elm::CategoryHeader("Configuration"));

        // 5. Elements (Mini/Micro only)
        if (isMiniMode || isMicroMode) {
            auto* showSettings = new tsl::elm::ListItem("Elements");
            showSettings->setValue(ult::DROPDOWN_SYMBOL);
            showSettings->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<ShowConfig>(modeName);
                    return true;
                }
                return false;
            });
            list->addItem(showSettings);
        }

        // 3. Toggles (All modes)
        //if (isMiniMode || isMicroMode || isFullMode || isFPSGraphMode) {
        auto* toggles = new tsl::elm::ListItem("Toggles");
        toggles->setValue(ult::DROPDOWN_SYMBOL);
        toggles->setClickListener([this](uint64_t keys) {
            if (keys & KEY_A) {
                tsl::changeTo<TogglesConfig>(modeName);
                return true;
            }
            return false;
        });
        list->addItem(toggles);
        //}

        // 2. Colors (not Full mode - it has no color settings)
        //if (!isFullMode) {
        auto* colors = new tsl::elm::ListItem("Colors");
        colors->setValue(ult::DROPDOWN_SYMBOL);
        colors->setClickListener([this](uint64_t keys) {
            if (keys & KEY_A) {
                tsl::changeTo<ColorConfig>(modeName);
                return true;
            }
            return false;
        });
        list->addItem(colors);
        //}


        // 4. Font Sizes (Mini/Micro/FPS Counter only)
        if (isMiniMode || isMicroMode || isFPSCounterMode) {
            auto* fontSizes = new tsl::elm::ListItem("Font Sizes");
            fontSizes->setValue(ult::DROPDOWN_SYMBOL);
            fontSizes->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<FontSizeConfig>(modeName);
                    return true;
                }
                return false;
            });
            list->addItem(fontSizes);
        }

        // 1. Refresh Rate (all modes)
        auto* refreshRate = new tsl::elm::ListItem("Refresh Rate");
        refreshRate->setValue(std::to_string(getCurrentRefreshRate()) + " Hz");
        refreshRate->setClickListener([this](uint64_t keys) {
            if (keys & KEY_A) {
                tsl::changeTo<RefreshRateConfig>(modeName);
                return true;
            }
            return false;
        });
        list->addItem(refreshRate);

        // 6. DTC Format (Mini/Micro only) - NEW ADDITION
        if (isMiniMode || isMicroMode) {
            auto* dtcFormat = new tsl::elm::ListItem("DTC Format");
            dtcFormat->setValue(getCurrentDTCFormat());
            dtcFormat->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<DTCFormatConfig>(modeName);
                    return true;
                }
                return false;
            });
            list->addItem(dtcFormat);
        }

        // 7. Frame Padding (Mini only) - NEW ADDITION
        if (isMiniMode || isGameResolutionsMode || isFPSCounterMode || isFPSGraphMode) {
            auto* framePadding = new tsl::elm::ListItem("Frame Padding");
            framePadding->setValue(std::to_string(getCurrentFramePadding()) + " px");
            framePadding->setClickListener([this](uint64_t keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<FramePaddingConfig>(modeName);
                    return true;
                }
                return false;
            });
            list->addItem(framePadding);
        }

        // 7. Mode-specific positioning settings
        if (isMicroMode) {
            // Text Alignment for Micro
            auto* textAlign = new tsl::elm::ListItem("Text Alignment");
            textAlign->setValue(getCurrentTextAlign());
            textAlign->setClickListener([this, textAlign](uint64_t keys) {
                if (keys & KEY_A) {
                    const std::string next = cycleTextAlign();
                    textAlign->setValue(next);
                    return true;
                }
                return false;
            });
            list->addItem(textAlign);

            // Vertical Position for Micro (Top/Bottom only)
            auto* layerPos = new tsl::elm::ListItem("Vertical Position");
            layerPos->setValue(getCurrentLayerPosBottom());
            layerPos->setClickListener([this, layerPos](uint64_t keys) {
                if (keys & KEY_A) {
                    const std::string next = cycleLayerPosBottom();
                    layerPos->setValue(next);
                    return true;
                }
                return false;
            });
            list->addItem(layerPos);

        } else if (isFullMode) {
            // Horizontal Position for Full (Left/Right only)
            auto* layerPos = new tsl::elm::ListItem("Horizontal Position");
            layerPos->setValue(getCurrentLayerPosRight());
            layerPos->setClickListener([this, layerPos](uint64_t keys) {
                if (keys & KEY_A) {
                    const std::string next = cycleLayerPosRight();
                    layerPos->setValue(next);
                    return true;
                }
                return false;
            });
            list->addItem(layerPos);

        //} else if (isGameResolutionsMode || isFPSCounterMode || isFPSGraphMode) {
        //    // Both horizontal and vertical positioning
        //    auto* layerPosH = new tsl::elm::ListItem("Horizontal Position");
        //    layerPosH->setValue(getCurrentLayerPosRight());
        //    layerPosH->setClickListener([this, layerPosH](uint64_t keys) {
        //        if (keys & KEY_A) {
        //            const std::string next = cycleLayerPosRight();
        //            layerPosH->setValue(next);
        //            return true;
        //        }
        //        return false;
        //    });
        //    list->addItem(layerPosH);
        //
        //    auto* layerPosV = new tsl::elm::ListItem("Vertical Position");
        //    layerPosV->setValue(getCurrentLayerPosBottom());
        //    layerPosV->setClickListener([this, layerPosV](uint64_t keys) {
        //        if (keys & KEY_A) {
        //            const std::string next = cycleLayerPosBottom();
        //            layerPosV->setValue(next);
        //            return true;
        //        }
        //        return false;
        //    });
        //    list->addItem(layerPosV);
        }

        list->jumpToItem(jumpItemName, jumpItemValue, jumpItemExactMatch.load(std::memory_order_acquire));
        {
            jumpItemName = "";
            jumpItemValue = "";
            jumpItemExactMatch = false;
        }

        tsl::elm::OverlayFrame* rootFrame = new tsl::elm::OverlayFrame("Horizon OC Monitor", modeName);
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysDown & KEY_B) {
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            lastSelectedItem = modeName;
            tsl::swapTo<MainMenu>();
            return true;
        }
        return false;
    }

private:
    int getCurrentRefreshRate() {
        std::string section;
        if (isMiniMode) section = "mini";
        else if (isMicroMode) section = "micro";
        else if (isFullMode) section = "full";
        else if (isGameResolutionsMode) section = "game_resolutions";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";

        std::string value = ult::parseValueFromIniSection(configIniPath, section, "refresh_rate");
        int defaultRate = (isGameResolutionsMode) ? 10 : ((isFPSCounterMode || isFPSGraphMode) ? 30 : 1);
        return value.empty() ? defaultRate : atoi(value.c_str());
    }

    // NEW METHOD: Get current DTC format for display
    std::string getCurrentDTCFormat() {
        if (isMiniMode || isMicroMode) {
            const std::string section = isMiniMode ? "mini" : "micro";
            std::string value = ult::parseValueFromIniSection(configIniPath, section, "dtc_format");

            // Handle defaults properly
            if (value.empty()) {
                value = isMiniMode ? "%m-%d-%Y"+ult::DIVIDER_SYMBOL+"%H:%M:%S" : "%H:%M:%S";
            }

            // Convert format string to display name
            return getDTCFormatName(value);
        }
        return "";
    }

    // Helper function to convert format string to display name
    std::string getDTCFormatName(const std::string& formatStr) {

        for (const auto& format : dtcFormats) {
            if (format.second == formatStr) {
                return format.first;
            }
        }

        // Return the format string itself if no match found
        return formatStr;
    }

    int getCurrentFramePadding() {
        if (isMiniMode) {
            std::string value = ult::parseValueFromIniSection(configIniPath, "mini", "frame_padding");
            return value.empty() ? 10 : atoi(value.c_str());
        }
        return 10;
    }

    std::string getCurrentTextAlign() {
        if (isMicroMode) {
            std::string value = ult::parseValueFromIniSection(configIniPath, "micro", "text_align");
            convertToUpper(value);
            if (value == "LEFT") return "Left";
            if (value == "RIGHT") return "Right";
            return "Center";
        }
        return "";
    }

    std::string getCurrentLayerPosRight() {
        std::string section;
        if (isFullMode) section = "full";
        else if (isGameResolutionsMode) section = "game_resolutions";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";

        std::string value = ult::parseValueFromIniSection(configIniPath, section, "layer_width_align");
        convertToUpper(value);

        if (isFullMode) {
            // Full mode: only Left and Right allowed
            if (value == "RIGHT") return "Right";
            return "Left";
        } else {
            // Other modes: allow Center
            if (value == "RIGHT") return "Right";
            if (value == "CENTER") return "Center";
            return "Left";
        }
    }

    std::string getCurrentLayerPosBottom() {
        std::string section;
        if (isMicroMode) section = "micro";
        else if (isGameResolutionsMode) section = "game_resolutions";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";

        std::string value = ult::parseValueFromIniSection(configIniPath, section, "layer_height_align");
        convertToUpper(value);

        if (isMicroMode) {
            // Micro mode: only Top and Bottom allowed
            if (value == "BOTTOM") return "Bottom";
            return "Top";
        } else {
            // Other modes: allow Center
            if (value == "BOTTOM") return "Bottom";
            if (value == "CENTER") return "Center";
            return "Top";
        }
    }

    std::string cycleTextAlign() {
        if (isMicroMode) {
            const std::string current = getCurrentTextAlign();
            std::string next;
            if (current == "Left") next = "Center";
            else if (current == "Center") next = "Right";
            else if (current == "Right") next = "Left";
            else next = "Center";

            ult::setIniFileValue(configIniPath, "micro", "text_align", next);
            return next;
        }
        return "";
    }

    std::string cycleLayerPosRight() {
        std::string section;
        if (isFullMode) section = "full";
        else if (isGameResolutionsMode) section = "game_resolutions";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";

        const std::string current = getCurrentLayerPosRight();
        std::string next;

        if (isFullMode) {
            // Full mode: only Left and Right
            if (current == "Left") next = "Right";
            else next = "Left";
        } else {
            // Other modes: Left -> Center -> Right -> Left
            if (current == "Left") next = "Center";
            else if (current == "Center") next = "Right";
            else next = "Left";
        }

        const std::string value = (next == "Right") ? "right" : (next == "Center" ? "center" : "left");
        ult::setIniFileValue(configIniPath, section, "layer_width_align", value);
        return next;
    }

    std::string cycleLayerPosBottom() {
        std::string section;
        if (isMicroMode) section = "micro";
        else if (isGameResolutionsMode) section = "game_resolutions";
        else if (isFPSCounterMode) section = "fps-counter";
        else if (isFPSGraphMode) section = "fps-graph";

        const std::string current = getCurrentLayerPosBottom();
        std::string next;

        if (isMicroMode) {
            // Micro mode: only Top and Bottom
            if (current == "Top") next = "Bottom";
            else next = "Top";
        } else {
            // Other modes: Top -> Center -> Bottom -> Top
            if (current == "Top") next = "Center";
            else if (current == "Center") next = "Bottom";
            else next = "Top";
        }

        const std::string value = (next == "Bottom") ? "bottom" : (next == "Center" ? "center" : "top");
        ult::setIniFileValue(configIniPath, section, "layer_height_align", value);
        return next;
    }
};
