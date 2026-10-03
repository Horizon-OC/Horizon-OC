Horizon OC Compilation Instructions

1. Install devkitpro (https://devkitpro.org/wiki/Getting_Started) with switch-dev
2. Set up a development enviorment for compiling Atmosphere (https://github.com/Atmosphere-NX/Atmosphere/blob/master/docs/building.md)
3. Clone the Horizon OC develop branch (``git clone https://github.com/Horizon-OC/Horizon-OC.git --recurse-submodules``)
4. Run ``./build.sh`` in the root directory
 - If you want to compile with extensions, append ``--ext``
 - To skip components, append ``-s``/``--skip`` followed by one or more of: ``ldr`` (loader), ``clk`` (hoc-clk), ``hek`` (hekate), ``btb`` (benchmark toolbox), ``exos`` (exosphere), ``sm`` (status-monitor download), ``zip`` (dist.zip packaging). Example: ``./build.sh --ext -s ldr hek``
 - Building the custom Hekate (``--ext`` without skipping ``hek``) additionally requires devkitARM with ``DEVKITARM`` exported
 - Installing ``ccache`` is optional; if it is on your PATH it is used automatically to speed up rebuilds
