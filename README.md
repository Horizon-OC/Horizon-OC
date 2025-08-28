# Switch OC Suite
[![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![Downloads](https://img.shields.io/github/downloads/hanai3Bi/Switch-OC-Suite/total)](https://github.com/hanai3Bi/Switch-OC-Suite/releases)

 
     
*If Switch OC Suite is so good, then why isn't there a Switch OC Suite 2?*

This project is very dangerous and can possibly damage your console. If you decide to use it, USE AT YOUR OWN RISK. THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND.

Overclocking Suite for Nintendo Switch consoles running Atmosphere CFW.

[Project Homepage](https://hanai3Bi.github.io/Switch-OC-Suite)

**DISCLAIMER: USE AT YOUR OWN RISK!**

- Overclocking in general will shorten the lifespan of some hardware components. **YOU ARE RESPONSIBLE for any problem or potential damage** if unsafe frequencies are ENABLED in sys-clk-OC. Issues like asking for bypassing limit will BE IGNORED OR CLOSED WITHOUT REPLY.

- Due to HorizonOS design, instabilities from unsafe RAM clocks may cause filesystem corruption. **Always make backup before enabling DRAM OC.**

## Features

- Erista variant (HAC-001)
  - CPU / GPU Overclock (Safe: 1785 / 921 MHz)
    - Unsafe
      - Due to the limit of board power draw or power IC
      - Unlockable frequencies up to 2091 / 998 MHz
      - See [README for sys-clk-OC](https://github.com/hanai3Bi/Switch-OC-Suite/blob/master/Source/sys-clk-OC/README.md)

  - DRAM Overclock (Safe: 1862.4 MHz)

- Mariko variant (HAC-001-01, HDH-001, HEG-001)
  - CPU / GPU Overclock (Safe: 1963 / 998 MHz)
    - Unsafe
      - Due to the limit of board power draw or power IC
      - Unlockable frequencies up to 2295 / 1267 MHz
      - See [README for sys-clk-OC](https://github.com/hanai3Bi/Switch-OC-Suite/blob/master/Source/sys-clk-OC/README.md)

  - DRAM Overclock (Safe: 1996.8 MHz)

- Modded sys-clk
  - Global Profile
    - Designated a dummy title id `0xA111111111111111`.
    - Priority: "Temp overrides" > "Application profile" > "Global profile" > "System default".
  - Real Voltage readings (CPU/GPU/RAM)
  - Bug Fixes

- **[System Settings (Optional)](https://github.com/hanai3Bi/Switch-OC-Suite/blob/master/system_settings.md)**


## Installation

1. Download latest [release](https://github.com/hanai3Bi/Switch-OC-Suite/releases).

2. Grab `x.x.x_loader.kip` for your Atmosphere version, rename it to `loader.kip` and place it in `/atmosphere/kips/`.

3. (optional) You can customize via [online loader configurator](https://hanai3Bi.github.io/Switch-OC-Suite/#config):
    <details>

    | Defaults   | Mariko        | Erista        |
    | ---------- | ------------- | ------------- |
    | CPU OC     | 2295 MHz Max  | 2091 MHz Max  |
    | CPU Volt   | 1235 mV       | 1235 mV       |
    | GPU OC     | 1267 MHz Max  | 998 Mhz max   |
    | RAM OC     | 1996 MHz      | 1862 MHz      |
    | RAM Volt   | Disabled      | Disabled      |
    | RAM Timing | Auto-Adjusted | Auto-Adjusted |
    | CPU UV     | Disabled      | N/A           |
    | GPU UV     | Disabled      | N/A           |

    </details>

4. Hekate-ipl bootloader (fss0) Only  (Not required for AMS fusee)
   - At boot entry section in `bootloader/hekate_ipl.ini`, Add `kip1=atmosphere/kips/loader.kip` to any line that works.

5. Install [sys-clk-oc](https://github.com/hanai3Bi/Switch-OC-Suite/releases/latest/download/sys-clk-oc.zip)
   - official [sys-clk](https://github.com/retronx-team/sys-clk/releases) (2.0.0+) is compatible, but not recommended (no bugfixes or additional features).

6. (optional) Copy SdOut.zip for useful utilities.

## Updating via AIO

1. Download and copy custom_packs.json to /config/aio-switch-updater/custom_packs.json

2. Launch AIO Switch Updater and go to Custom Downloads tab

3. Select Switch-OC-Suite and press Contiune 


## Build

<details>

1. Copy Switch-OC-Suite files into Atmosphere folder. 

2. Before compiling, run `patch.py` in `Atmosphere/stratosphere/loader/source/` to insert oc module into loader sysmodule.

3. Compile Atmosphere loader with devkitpro.

4. When compilation is done, uncompress the kip to make it work with configurator: `hactool -t kip1 Atmosphere/stratosphere/loader/out/nintendo_nx_arm64_armv8a/release/loader.kip --uncompress=./loader.kip`

</details>


## Acknowledgement

- CTCaer for [Hekate-ipl](https://github.com/CTCaer/hekate) bootloader, RE and hardware research
- [devkitPro](https://devkitpro.org/) for All-In-One homebrew toolchains
- masagrator for [ReverseNX-RT](https://github.com/masagrator/ReverseNX-RT) and info on BatteryChargeInfoFields in psm module
- Nvidia for [Tegra X1 Technical Reference Manual](https://developer.nvidia.com/embedded/dlc/tegra-x1-technical-reference-manual)
- RetroNX team for [sys-clk](https://github.com/retronx-team/sys-clk)
- SciresM and Reswitched Team for the state-of-the-art [Atmosphere](https://github.com/Atmosphere-NX/Atmosphere) CFW of Switch
- Switchbrew [wiki](http://switchbrew.org/wiki/) for Switch in-depth info
- Switchroot for their [modified L4T kernel and device tree](https://gitlab.com/switchroot/kernel)
- ZatchyCatGames for RE and original OC loader patches for Atmosphere
- KazushiMe for [Switch-OC-Suite](https://github.com/KazushiMe/Switch-OC-Suite)
- lineon for research and help
