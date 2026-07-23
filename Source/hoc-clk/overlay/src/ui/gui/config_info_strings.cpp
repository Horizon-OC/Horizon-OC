/*
 *
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

#include "config_info_strings.h"

std::vector<std::string> ConfigInfoStrings(HocClkConfigValue val, bool isMariko, bool isHoag)
{
    switch (val)
    {
        case HocClkConfigValue_PollingIntervalMs:
            return {
                "The interval (in milliseconds) where clocks are applied, temperatures and voltages are polled and logs are written (if enabled).",
                "Higher values may cause more delay between changing a setting and it taking effect, and lower values may increase sysmodule memory usage",
                "Default: 300ms"
            };

        case HocClkConfigValue_RamDisplayUnit:
            return {
                "The unit used when displaying RAM frequency values.",
                "Options:",
                "- MHz: Megahertz (e.g. 1600 MHz)",
                "- MT/s: MegaTransfers per second (e.g. 3200 MT/s)",
                "- MHz and MT/s: Display in both MHz and MT/s",
                "Default: MHz"
            };

        case HocClkConfigValue_RAMVoltDisplayMode:
            return {
                "The method used to display RAM voltage values.",
                "Options:",
                "- VDD2 - Display VDD2 voltage",
                "- VDDQ - Display VDDQ voltage",
                "Default: VDD2"
            };

        case HocClkConfigValue_EnableExperimentalSettings:
            return {
                "When enabled, shows settings that are still being tested and may be unstable or not work at all.",
                "Use with caution and report any issues to the developers."
            };

        case HocClkConfigValue_MarikoMiddleFreqs:
            return {
                "Allows usage of frequencies stepped by 38.4MHz instead of 76.8MHz below 1228MHz GPU clock",
                "Default: OFF"
            };

        case HocClkConfigValue_LiveCpuUv:
            return {
                "Allows changing CPU undervolt settings without a reboot",
                "Default: OFF"
            };

        case HocClkConfigValue_GPUSchedulingMethod:
            return {
                "Method used for GPU scheduling override",
                "Options:",
                "- INI: override via system_settings.ini",
                "- NV Service: override via nvservices sysmodule (experimental)",
                "Default: INI"
            };

        case HocClkConfigValue_MemoryFrequencyMeasurementMode:
            return {
                "How the RAM real frequency is measured",
                "Options:",
                "- PLL: Measure from PLLMB and PLLM (more accurate)",
                "- Actmon: Measure from Actmon (less accurate)",
                "Default: PLL"
            };

        case HocClkConfigValue_BatteryChargeCurrent:
            return {
                "Overrides the charge current to the battery. Use with caution!",
                isHoag ? "Default: 1664 mA" : "2048 mA"
            };


        case HocClkConfigValue_InputCurrentLimit:
            return {
                "Overrides the maximum input current from the charger.",
                isHoag ? "Default: 900 mA" : "1200 mA"
            };

        case HocClkConfigValue_AulaDisplayColorPreset:
            return {
                "Current display color preset. Default is Basic",
                "Options:",
                "- Saturated: Based on Vivid but over-saturated.",
                "- Washed: Washed out colors.",
                "- Basic: Real natural profile.",
                "- Natural: Not actually natural.. Extra saturation.",
                "- Vivid: Saturated.",
                "Default: Do not override"
            };

        case HocClkConfigValue_CpuGovernorMinimumFreq:
            return {
                "The minimum frequency that the CPU governor will allow.",
                "Default: 612MHz"
            };

        case HocClkConfigValue_OverwriteRefreshRate:
            return {
                "Controls the availability of display refresh rate features.",
                "When enabled, allows changing the display refresh rate and using display refresh rate related features.",
                "This feature conflicts with FPSLocker's feature that does the same thing.",
                "Default: OFF"
            };

        case HocClkConfigValue_MaxDisplayClockH:
            return {
                "The maximum display clock frequency in handheld mode (in Hz).",
                "Warning: Changing this value may cause instability or display damage.",
                "Default: 60 Hz"
            };

        case HocClkConfigValue_DisplayVoltage:
            return {
                "The voltage supplied to the display panel (in mV).",
                "Warning: Changing this value may cause instability.",
                "Default: 1200mV"
            };

        case HocClkConfigValue_UncappedClocks:
            if(isMariko) {
                return {
                    "When enabled, disables clock cappings",
                    "Warning: Enabling this may cause damage to your device without a proper undervolt. Use with caution!",
                    "Clock cappings:",
                    "- Handheld:",
                    "  - GPU (No UV): 614 MHz",
                    "  - GPU (SLT): 691 MHz",
                    "  - GPU (HiOPT): 768 MHz",
                    "  - GPU (HiOPT - 15mV): 844 MHz",
                    "  - GPU (High UV): 921 MHz",
                    "- USB Charger",
                    "  - GPU (No UV): 844 MHz",
                    "  - GPU (SLT): 921 MHz",
                    "  - GPU (HiOPT): 998 MHz",
                    "  - GPU (HiOPT - 15mV): 1075 MHz",
                    "  - GPU (High UV): 1152 MHz",
                    "- PD Charger / Docked:",
                    "  - No capping applied",
                    "Default: OFF"
                };
            } else {
                return {
                    "When enabled, disables clock cappings",
                    "Warning: Enabling this may cause damage to your device without a proper undervolt. Use with caution!",
                    "Clock cappings:",
                    "- Handheld:",
                    "  - GPU: 460 MHz",
                    "  - CPU: 1581 MHz",
                    "- USB Charger",
                    "  - GPU: 768 MHz",
                    "- PD Charger / Docked:",
                    "  - No capping applied",
                    "Default: OFF"
                };
            }

        case HocClkConfigValue_ThermalThrottle:
            return {
                "If enabled, Resets to stock clocks after the threshold is applied",
                "Default: ON",
            };

        case HocClkConfigValue_ThermalThrottleThreshold:
            return {
                "The temperature threshold (in °C) for resetting to stock clocks when Thermal Throttle is enabled.",
                "Default: 70°C"
            };

        case KipConfigValue_emcDvbShift:
            return {
                "Each level adds/removes 25mV from the SOC Voltage table",
                "Consoles are bracketed by SoC Speedo. The brackets are as follows:",
                " - Speedo 1487-1598: Bracket 0",
                " - Speedo 1598-1709: Bracket 1",
                " - Speedo 1709-1820: Bracket 2",
                "Default: 0"
            };

        case KipConfigValue_marikoSocVmax:
            return {
                "The maximum available SOC Voltage that the DVB-adjusted table can use",
                "Default: Do not override"
            };

        case KipConfigValue_hpMode:
            return {
                "When enabled, disables RAM powerdown. Helps with latency significantly",
                "Default: OFF"
            };

        case KipConfigValue_commonEmcMemVolt:
            return {
                "RAM VDD2 voltage",
                "Increasing this WILL NOT increase your maximum frequency, but may help with timing reduction.",
                "Undervolting RAM is pointless and may hurt performance and stability",
                "Default: 1175 mV"
            };

        case KipConfigValue_marikoEmcVddqVolt:
            return {
                "RAM VDDQ voltage",
                "Increasing this may help, but the default value is usually good enough.",
                "Undervolting RAM is pointless and may hurt performance and stability",
                "Default: 600 mV"
            };

        case KipConfigValue_stepMode:
            return {
                "The step that RAM clocks take.",
                "Options (with examples):",
                " - 66MHz - 66 MHz step (ex. 1600, 1666, 1733, etc.)",
                " - 100MHz - 100 MHz step (ex. 1600, 1700, 1800, etc.)",
                " - 133MHz - 66 MHz step (ex. 1600, 1733, 1866, etc.)",
                " - JEDEC:",
                "   - 1600, 1866, 1996, 2133, 2400, 2666, 2933 and 3200 MHz are used",
                "The RAM max clock will always be available regardless of the step mode, but the intermediate frequencies will be limited by the selected step mode.",
                "This setting does not affect performance and the option you choose mostly is based on your personal taste",
                "33 MHz step mode is not possible due to certain limitations of Horizon OS",
                "Default: 66 MHz",
            };

        case KipConfigValue_marikoEmcMaxClock:
            return {
                "The maximum RAM frequency available.",
                "Higher frequencies may cause instability, so increase this gradually and test for stability.",
                "Default: 2133 MHz"
            };

        case KipConfigValue_eristaEmcMaxClock:
            return {
                "The RAM frequency used in the particular slot. Higher frequencies may cause instability, so increase this gradually and test for stability.",
                "Default: Disabled (1600 MHz)"
            };

        case KipConfigValue_t1_tRCD:
            return {
                "RAS-to-CAS delay",
                "Default: 0"
            };

        case KipConfigValue_t2_tRP:
            return {
                "Row precharge time",
                "Default: 0"
            };

        case KipConfigValue_t3_tRAS:
            return {
                "Row active time",
                "Default: 0"
            };

        case KipConfigValue_t4_tRRD:
            return {
                "Row refresh time",
                "Default: 0"
            };

        case KipConfigValue_t5_tRFC:
            return {
                "Refresh Cycle Time",
                "Default: 0"
            };

        case KipConfigValue_t6_tRTW:
            return {
                "Read To Write (High bracket)",
                "Default: 0"
            };

        case KipConfigValue_t7_tWTR:
            return {
                "Write To Read (High bracket)",
                "Default: 0"
            };

        case KipConfigValue_t8_tREFI:
            return {
                "Refresh command interval",
                "Default: 0"
            };

        case KipConfigValue_timingEmcTbreak:
            return {
                "Frequency where t6 and t7 break between the low and high brackets",
                "Example:",
                "Tbreak is set to 1866 MHz, and t6Low is set to 4 and t6High is set to 2. At frequencies below 1866 MHz, t6 will be 4, and at frequencies above 1866 MHz, t6 will be 2.",
                "Default: Disabled"
            };

        case KipConfigValue_low_t6_tRTW:
            return {
                "Read To Write (Low bracket)",
                "Default: 0"
            };

        case KipConfigValue_low_t7_tWTR:
            return {
                "Write To Read (Low bracket)",
                "Default: 0"
            };
        case KipConfigValue_t6_tRTW_fine_tune:
            return {
                "Fine-tunes the raw calculation of t6",
                "Default: 0"
            };

        case KipConfigValue_t7_tWTR_fine_tune:
            return {
                "Fine-tunes the raw calculation of t7",
                "Default: 0"
            };

        case KipConfigValue_write_latency_1333:
        case KipConfigValue_write_latency_1600:
        case KipConfigValue_write_latency_1866:
        case KipConfigValue_write_latency_2133:
        case KipConfigValue_read_latency_1333:
        case KipConfigValue_read_latency_1600:
        case KipConfigValue_read_latency_1866:
        case KipConfigValue_read_latency_2133:
            return {
                "Latency bracket settings",
                "Example:",
                "If 1333 is set to 2000 MHz, 1600 set to 2500 MHz, 1866 set to 2766 MHz and 2133 set to 2933 MHz:",
                "Frequencies below 2000 MHz use 1333, 2033-2500 MHz use 1600, 2533-2766 MHz use 1866 and 2800-2933 MHz use 2133. ",
                "Either of these can be omitted and it will work (say you set 1333 to -, then <2000 MHz will use 1600 latency)",
                "If all of these parameters are omitted the latency will automatically be calculated as follows:",
                "1633-1866 MHz - 1866 WRL",
                "1900+ MHz - 2133 WRL",
                "These properties apply for both write and read latencies, and you can mix-and-match the brackets if necessary",
                "Default: -"
            };
        case KipConfigValue_marikoCpuUVLow:
            return {
                "The CPU UV level used before tBreak",
                "Default: 0"
            };

        case KipConfigValue_marikoCpuUVHigh:
            return {
                "The CPU UV level used after tBreak",
                "Default: 0"
            };

        case KipConfigValue_tableConf:
            return {
                "The current UV table used. The tbreaks are as follows:",
                "1581 MHz tBreak and 1683 MHz tBreak use their respective tBreaks",
                "The other tables use 1581 MHz as tBreak",
                "The \"Default\" table does not contain frequencies past 1963 MHz and may not undervolt correctly"
            };

        case KipConfigValue_marikoCpuLowVmin:
            return {
                "The CPU vmin used before tBreak",
                "Default: 620 mV"
            };

        case KipConfigValue_marikoCpuHighVmin:
            return {
                "The CPU vmin used after tBreak",
                "Default: 750 mV"
            };


        case KipConfigValue_marikoCpuMaxVolt:
            return {
                "The maximum voltage that the CPU can use",
                "Change this setting with caution!",
                "Default: 1120 mV"
            };

        case KipConfigValue_marikoCpuMaxClock:
            return {
                "The maximum available CPU clock",
                "Default: 1963 MHz"
            };

        case KipConfigValue_marikoCpuBoostClock:
            return {
                "The clock used for the CPU in \"boost mode\"",
                "Default: 1963 MHz"
            };

        case KipConfigValue_eristaCpuUV:
            return {
                "CPU undervolt level",
                "Default: 0"
            };

        case KipConfigValue_eristaCpuUnlock:
            return {
                "Unlock unsafe CPU clocks",
                "Default: OFF"
            };

        case KipConfigValue_eristaCpuVmin:
            return {
                "Minimum CPU voltage",
                "Default: 825 mV"
            };

        case KipConfigValue_eristaCpuMaxVolt:
            return {
                "Maximum CPU voltage",
                "Default: 1235 mV"
            };

        case HocClkConfigValue_EristaMaxCpuClock:
            return {
                "The maximum available CPU clock",
                "Default: 1785 MHz"
            };

        case KipConfigValue_eristaCpuBoostClock:
            return {
                "The clock used for the CPU in \"boost mode\"",
                "Default: 1785 MHz"
            };
        case HocClkConfigValue_AutoRAMCPUOverclock:
            return {
                "When enabled, automatically raises the CPU clock to the configured OC frequency when RAM clock meets or exceeds the threshold to meet the increased voltage requirement.",
                "Default: ON"
            };

        case HocClkConfigValue_AutoRamCpuCpuOCFreq:
            return {
                "The CPU clock (in MHz) applied when Auto High RAM CPU OC is enabled and the RAM threshold is met.",
                "Default: 1683 MHz"
            };

        case HocClkConfigValue_AutoRamCpuRamOCThreshold:
            return {
                "The RAM clock threshold (in MHz) at or above which the Auto High RAM CPU OC will activate.",
                "Default: 2133MHz"
            };

        case HocClkConfigValue_OverwriteBoostMode:
            return {
                "If enabled, profiles can override the boost mode setting",
                "Default: OFF"
            };

        case KipConfigValue_marikoGpuUV:
            return {
                "GPU undervolt level",
                "Options:",
                " - No Undervolt: No Undervolt, HOS default",
                " - SLT Table: NVIDIA custom SLT Table",
                " - HiOPT: L4T Custom HiOPT table, HOC Default",
                " - HiOPT - 15mV: L4T Custom HiOPT table with a 15mV offset",
                " - High UV: The highest undervolt table",
                "Default: HiOPT"
            };

        case KipConfigValue_marikoGpuVmin:
            return {
                "Minimum GPU voltage",
                "Note: DVFS may change this value when the RAM clock is changed if the DVFS mode is set to PCV Hijack",
                "Default: 610 mV"
            };

        case KipConfigValue_marikoGpuVmax:
            return {
                "Maximum GPU voltage",
                "Default: 800 mV"
            };

        case HocClkConfigValue_DVFSMode:
            return {
                "The mode used for GPU DVFS",
                "Adjusts GPU vmin when RAM clock is changed due to a higher requirement",
                "Options:",
                "- Disabled: disabled...",
                "- PCV Hijack: hijack PCV for override",
                "Default: PCV Hijack"
            };

        case HocClkConfigValue_DVFSOffset:
            return {
                "The offset added/subtracted to the GPU vmin when the RAM clock is changed due to a higher requirement in PCV Hijack mode",
                "Default: 0 mV (Disabled)"
            };

        case KipConfigValue_eristaGpuUV:
            return {
                "GPU undervolt level",
                "Options:",
                " - No Undervolt: No Undervolt...",
                " - SLT Table: NVIDIA custom SLT Table",
                " - HiOPT: L4T Custom HiOPT table",
                "Default: No Undervolt"
            };

        case KipConfigValue_eristaGpuVmin:
            return {
                "Minimum GPU voltage",
                "Default: 810 mV (812mV as erista is stepped by 6.5mV instead of 5mV)"
            };

        case KipConfigValue_commonGpuVoltOffset:
            return {
                "The offset added/subtracted to all GPU voltages marked as \"auto\"",
                "Default: 0 mV (Disabled)"
            };

        case HocClkConfigValue_GPUScheduling:
            return {
                "The scheduling method used for GPU clocks",
                "Options:",
                "- Do Not Override: Do not override existing scheduling mode",
                "- Disabled: Disables GPU scheduling, 99.7% GPU max load",
                "- Enabled: Enables GPU scheduling, 96.5% GPU max load",
                "Default: Do not override"
            };
        case HocClkConfigValue_LiveGpuVoltage:
            return {
                "Adjust GPU voltage on the fly.",
                "Warning: unstable settings allowed.",
                "Exception: while RAM OC is active, voltage cannot drop below stability minimum.",
                "RAM frequency changes overwrite requested voltage.",
                "Voltage applies globally across all frequencies.",
                "\"Reset\" restores default or RAM-stable minimum."
            };
        default:
            return {};
    }
}