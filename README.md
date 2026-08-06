
<div align="center">

<img src="assets/logo.png" alt="logo" width="768"/>

---

![License: GPL-2.0](https://img.shields.io/badge/GPL--2.0-red?style=for-the-badge)
![Nintendo Switch](https://img.shields.io/badge/Nintendo_Switch-E60012?style=for-the-badge\&logo=nintendo-switch\&logoColor=white)
[![Discord](https://img.shields.io/badge/Discord-5865F2?style=for-the-badge\&logo=discord\&logoColor=white)](https://dsc.gg/horizonoc)
![VSCode](https://img.shields.io/badge/VSCode-0078D4?style=for-the-badge\&logo=visual%20studio%20code\&logoColor=white)
![Made with Notepad++](assets/np++.png?raw=true)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge\&logo=c%2B%2B\&logoColor=white)
![Downloads](https://img.shields.io/github/downloads/Horizon-OC/Horizon-OC/total.svg?style=for-the-badge)

---

</div>

## ⚠️ Disclaimer

> **THIS TOOL CAN BE DANGEROUS IF MISUSED. PROCEED WITH CAUTION.**
> Due to the design of Horizon OS, **overclocking RAM can cause NAND OR SD CORRUPTION.**
> Ensure you have a **full NAND, PRODINFO, EMUMMC and SD backup** before proceeding.

---

## About

**Horizon OC** is an open-source overclocking tool for Nintendo Switch consoles running **Atmosphere custom firmware**.
It enables advanced CPU, GPU, and RAM tuning with user-friendly configuration tools.

---

## Default clocks

* **CPU:** Up to 1963MHz (Mariko) / 1785MHz (Erista)
* **GPU:** Up to 1152MHz (Mariko) / 921MHz (Erista)
* **RAM:** Up to 2133MHz (Mariko) / 1600MHz (Erista)
* Over/undervolting support
* Built-in configurator
* Compatible with most homebrew

> It is recommended to read the [guide](https://rentry.co/howtoget60fps) before proceeding, as this can help you get a *significant* performance boost over the default settings, often times with less power draw and heat output

---

## Installation

1. Ensure you have the latest versions of

   * [Atmosphere](https://github.com/Atmosphere-NX/Atmosphere)
   * [Ultrahand Overlay](https://github.com/ppkantorski/Ultrahand-Overlay)
2. Download and extract the **Horizon OC Package** to the root of your SD card.
3. If using **Hekate**, edit `hekate_ipl.ini` to include:

   ```
   kip1=atmosphere/kips/hoc.kip
   secmon=atmosphere/exosphere.bin
   ```

   *(No changes needed if using fusee.)*

---

## Configuration

1. Open the Horizon OC Overlay
2. Open the settings menu
3. Adjust your overclocking settings as desired. A helpful guide can be found [here.](https://rentry.co/mariko#oc-settings-for-horizon-oc)
4. Click **Save KIP Settings** to apply your configuration.

---

## Donating

We don't accept donations. Put your spare cash into a charity instead :)

---

## Building from Source

Refer to COMPILATION.md

---

<details>
<summary><h2 style="display:inline">Clock table</h2></summary>

### MEM clocks (mhz)
* 3400 → max on mariko.
* 3366
* 3333
* 3300
* 3266
* 3233
* 3200 → JEDEC.
* 3166
* 3133
* 3100
* 3066
* 3033
* 3000
* 2966
* 2933 → JEDEC.
* 2900
* 2866
* 2833
* 2800
* 2766
* 2733
* 2700
* 2666 → JEDEC.
* 2633
* 2600
* 2566
* 2533
* 2500
* 2466
* 2433
* 2400 → max on erista, JEDEC.
* 2366
* 2333
* 2300
* 2266
* 2233
* 2200
* 2166
* 2133 → Mariko JEDEC standard max (4266 Modules)
* 2100
* 2066
* 2033
* 2000
* 1996 → JEDEC standard
* 1966
* 1933
* 1900
* 1866 → Mariko JEDEC standard max (3733 Modules)
* 1833
* 1800
* 1766
* 1733
* 1700
* 1666
* 1633
* 1600 → official docked, boost mode, Erista JEDEC standard max (3200 Modules), JEDEC.
* 1331 → official handheld, JEDEC.
* 1065 → Erista only
* 800  → Erista only
* 665  → Erista only
* 408  → Erista only
* 204  → sleep mode
* 102  → Erista only
* 68   → Erista only
* 40   → Erista only

### CPU clocks (mhz)
* 2703 → mariko absolute max, dangerous
* 2601 → unsafe
* 2499
* 2397 → mariko safe max with UV (low speedo)
* 2295
* 2193
* 2091
* 1963 → mariko no UV max clock
* 1887
* 1785 → erista no UV max clock, boost mode
* 1683
* 1581
* 1428
* 1326
* 1224 → sdev oc
* 1122
* 1020 → official docked & handheld
* 918
* 816
* 714
* 612 → sleep mode

### GPU clocks (mhz)
* 1536 → absolute max clock on mariko. Dangerous
* 1497 → max mariko unsafe clock
* 1459
* 1420
* 1382 → Max mariko "safe" clock
* 1344
* 1305
* 1267 → NVIDIA T214(mariko) rating
* 1228 → mariko High UV safe clock
* 1152 → mariko hiOpt-15mV max clock
* 1075 → mariko hiOpt max clock. absolute max clock on erista. very dangerous
* 998 → NVIDIA T210 (erista) rating
* 960 (erista only) → erista high uv/hiOpt-15mV safe max clock
* 921 → erista no UV max clock
* 844
* 768 → official docked
* 691
* 614
* 537
* 460 → max handheld
* 384 → official handheld
* 307 → official handheld
* 230
* 153
* 76 → boost mode

**Notes:**
1. On Erista, CPU in handheld is capped to 1581MHz
2. GPU overclock is capped at 460MHz on erista in handheld
3. On Mariko, cap with hiOpt is 614MHz, with hiOpt-15mV it is 691MHz and with High UV it's 768MHz
4. Clocks higher than 768MHz on erista need the official charger is plugged in.

</details>

---

## Credits
* **Lightos's Cat** - Cat
* **Souldbminer** - hoc-clk and loader development
* **Lightos** - Loader patches development, hoc-clk development, guides
* **TDRR** - HOC Logo Design
* **tetetete-ctrl** - Website design
* **SciresM** - Atmosphere CFW
* **CTCaer** - L4T, Hekate, proper RAM timings
* **KazushiMe** - Switch OC Suite
* **Hanai3bi (Meha)** - Switch OC Suite, EOS, sys-clk-eos
* **NaGaa95** - L4T-OC kernel, Status Monitor fork
* **B3711 (halop)** - EOS, contributions
* **sys-clk team (m4xw, p-sam, natinusala)** - sys-clk
* **Dominatorul** - Soctherm driver, guides, general help
* **ppkantorski** - Ultrahand sys-clk & Status Monitor fork
* **MasaGratoR and ZachyCatGames** - General help
* **MasaGratoR** - Status Monitor & Display Refresh Rate driver
* **Dominatorul, Samybigio, Arcdelta, Miki, Happy, Winnerboi77, Blaise, Alvise, agjeococh, frost, letum00, and Xenshen** - Testing
* **Samybigio2011, Miki** - Italian translations
* **angelblaster** - Korean translations
* **q1332348216-glitch, nangongjing1, xinluochenjiang** - Chinese translations
* **th3-ne0undr5c0r** - French translations
* **redraz** - Russian translations
* **Niklas080208** - JSON language support for sysmodule notifications.
* **Nvidia** - [Tegra X1 Technical Reference Manual](https://developer.nvidia.com/embedded/dlc/tegra-x1-technical-reference-manual), soctherm driver, L4T
