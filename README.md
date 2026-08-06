
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

See [CLOCKS.md](CLOCKS.md) for the full list of available MEM, CPU, and GPU clocks.

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
* **q1332348216-glitch** - Chinese translations
* **th3-ne0undr5c0r** - French translations
* **Nvidia** - [Tegra X1 Technical Reference Manual](https://developer.nvidia.com/embedded/dlc/tegra-x1-technical-reference-manual), soctherm driver, L4T
