#!/bin/sh

EXT=0
LDR_MAKE="nx_release"
LDR_SET=0
NO_EXO=0
JOBS=""
DEBUG_BUILD=0
SKIP=""

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DIST_DIR="$ROOT_DIR/dist"

while [ $# -gt 0 ]; do
    case "$1" in
        --ext)
            EXT=1
            ;;
        --ldr=*)
            LDR_MAKE="${1#*=}"
            LDR_SET=1
            ;;
        -d|--debug)
            DEBUG_BUILD=1
            ;;
        --no-exo)
            NO_EXO=1
            ;;
        -s|--skip)
            shift
            SKIP_GOT=0
            while [ $# -gt 0 ]; do
                case "$1" in
                    -*)
                        break
                        ;;
                    *)
                        SKIP="$SKIP $(printf '%s' "$1" | tr ',A-Z' ' a-z')"
                        SKIP_GOT=1
                        shift
                        ;;
                esac
            done
            if [ "$SKIP_GOT" -eq 0 ]; then
                echo "Missing value for -s/--skip: expected one or more of: ldr clk hek btb exos sm zip" >&2
                exit 1
            fi
            continue
            ;;
        --skip=*)
            SKIP_ADD="$(printf '%s' "${1#*=}" | tr ',A-Z' ' a-z')"
            if [ -z "$(printf '%s' "$SKIP_ADD" | tr -d ' ')" ]; then
                echo "Missing value for --skip: expected one or more of: ldr clk hek btb exos sm zip" >&2
                exit 1
            fi
            SKIP="$SKIP $SKIP_ADD"
            ;;
        -j)
            shift
            JOBS="$1"
            ;;
        -j*)
            JOBS="${1#-j}"
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
    shift
done

if [ "$DEBUG_BUILD" -eq 1 ] && [ "$LDR_SET" -eq 0 ]; then
    LDR_MAKE="nx_audit"
fi

should_skip() {
    case " $SKIP " in
        *" $1 "*)
            return 0
            ;;
    esac
    return 1
}

require_toolchain() {
    _tk_var="$1"
    _tk_gcc="$2"
    _tk_extra="$3"
    _tk_need="$4"
    eval "_tk_dir=\$$_tk_var"
    if [ -z "$_tk_dir" ] && [ -n "$DEVKITPRO" ]; then
        _tk_cand="$DEVKITPRO/devkit${_tk_var#DEVKIT}"
        if [ -d "$_tk_cand" ]; then
            eval "export $_tk_var=""$_tk_cand"""
            _tk_dir="$_tk_cand"
        fi
    fi
    if [ -z "$_tk_dir" ]; then
        echo "error: $_tk_var is not set, but it is required to build $_tk_need." >&2
        echo "Install devkitPro (https://devkitpro.org/wiki/Getting_Started) and export $_tk_var." >&2
        exit 1
    fi
    if [ ! -d "$_tk_dir" ]; then
        echo "error: $_tk_var points to a missing directory: $_tk_dir" >&2
        exit 1
    fi
    if [ -n "$_tk_extra" ] && [ ! -e "$_tk_dir/$_tk_extra" ]; then
        echo "error: $_tk_dir/$_tk_extra not found. your devkitPro install looks incomplete." >&2
        exit 1
    fi
    if ! command -v "$_tk_gcc" >/dev/null 2>&1 && [ ! -x "$_tk_dir/bin/$_tk_gcc" ]; then
        echo "error: $_tk_gcc not found. Add $_tk_dir/bin to your PATH." >&2
        exit 1
    fi
    unset _tk_var _tk_gcc _tk_extra _tk_need _tk_dir _tk_cand
}

for _s in $SKIP; do
    case "$_s" in
        ldr|clk|hek|btb|exos|sm|zip)
            ;;
        *)
            echo "Unknown skip component: $_s (valid components: ldr clk hek btb exos sm zip)" >&2
            exit 1
            ;;
    esac
done
unset _s

if should_skip exos; then
    NO_EXO=1
fi

_NEED_A64=0
if ! should_skip ldr; then _NEED_A64=1; fi
if ! should_skip clk; then _NEED_A64=1; fi
if [ "$EXT" -eq 1 ] && ! should_skip btb; then _NEED_A64=1; fi
if [ "$NO_EXO" -eq 0 ]; then _NEED_A64=1; fi
if [ "$_NEED_A64" -eq 1 ]; then
    require_toolchain DEVKITA64 aarch64-none-elf-gcc "" "loader/exosphere/hoc-clk/benchmark-toolbox (devkitA64)"
fi
unset _NEED_A64

LDR_BUILD_PATH="${LDR_MAKE#nx_}"

echo
if [ -n "$SKIP" ]; then
    echo "SKIP =$SKIP"
fi
if [ "$EXT" -eq 1 ]; then
    echo "EXT = 1"
fi

if [ "$NO_EXO" -eq 1 ]; then
    echo "NO_EXO = 1"
fi

if [ "$DEBUG_BUILD" -eq 1 ]; then
    echo "DEBUG_BUILD = 1 (loader build: $LDR_MAKE, debug build =)"
fi

CORES="$(nproc --all)"
echo "CORES: $CORES"
JOBS="${JOBS:-$CORES}"
echo "JOBS: $JOBS"

if command -v ccache >/dev/null 2>&1; then
    export ATMOSPHERE_CCACHE="ccache"
    export CCACHE_BASEDIR="$ROOT_DIR"
    export CCACHE_SLOPPINESS="time_macros,include_file_mtime,include_file_ctime,pch_defines,locale"
    export CCACHE_MAXSIZE="10G"
    echo "CCACHE: enabled ($(ccache --version | head -n1))"
else
    echo "CCACHE: not found, building without it"
fi

SRC="Source/Atmosphere/stratosphere/loader/"

mkdir -p "build"

ATMOSPHERE_DIR="build/atmosphere"
ATMOSPHERE_URL="https://github.com/Atmosphere-NX/Atmosphere.git"

if [ ! -d "$ATMOSPHERE_DIR" ]; then
    echo
    echo "*** Cloning atmosphere ***"
    git clone "$ATMOSPHERE_URL" "$ATMOSPHERE_DIR"

    cd "$ATMOSPHERE_DIR"

    git fetch --tags --quiet
    LATEST_TAG=$(git tag --sort=-version:refname | head -n1)
    git checkout -q -b build "$LATEST_TAG"

    cd "$ROOT_DIR"
fi

DEST="build/atmosphere/stratosphere/loader/"
mkdir -p "dist/atmosphere/kips/"
mkdir -p "$DEST"

echo
echo "*** Patching loader ***"
cp -vr "$SRC"/. "$DEST"/

if [ "$NO_EXO" -eq 0 ]; then
    echo
    echo "*** Patching exosphere ***"
    EXO_SRC="Source/Atmosphere-Patches"
    EXO_DEST="build/atmosphere/exosphere/program/source/smc"
    EXO_PROGRAM_DEST="build/atmosphere/exosphere/program/source"
    LIBEXO_DEST="build/atmosphere/libraries/libexosphere/include/exosphere/secmon"

    cp -v "$EXO_SRC/secmon_define_emc_access_table.inc"               "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_emc_access_table_data.inc"                 "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_define_emc01_access_table.inc"             "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_emc01_access_table_data.inc"               "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_define_rtc_pmc_access_table.inc"           "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_rtc_pmc_access_table_data.inc"             "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_smc_register_access.cpp"                   "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_smc_handler.cpp"                           "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_memory_layout.hpp"                         "$LIBEXO_DEST/"
fi

CCACHE_MKS="build/atmosphere/libraries/config/common.mk
build/atmosphere/libraries/config/templates/stratosphere.mk
build/atmosphere/libraries/libstratosphere/libstratosphere.mk"

for CCACHE_MK in $CCACHE_MKS; do
    if ! grep -q "ATMOSPHERE_CCACHE" "$CCACHE_MK"; then
        echo
        echo "*** Patching $CCACHE_MK for ccache ***"
        cat >> "$CCACHE_MK" <<'EOF'

ifneq ($(strip $(ATMOSPHERE_CCACHE)),)
ifneq ($(firstword $(CC)),$(ATMOSPHERE_CCACHE))
export CC  := $(ATMOSPHERE_CCACHE) $(CC)
endif
ifneq ($(firstword $(CXX)),$(ATMOSPHERE_CCACHE))
export CXX := $(ATMOSPHERE_CCACHE) $(CXX)
endif
endif
EOF
    fi
done

if should_skip ldr; then
    echo
    echo "*** Skipping loader ***"
else
    echo
    echo "*** Compiling loader ***"
    cd build/atmosphere/stratosphere/loader || exit 1
    make -j$JOBS HOC_UART_LOG=$DEBUG_BUILD "$LDR_MAKE"
    hactool -t kip1 "out/nintendo_nx_arm64_armv8a/$LDR_BUILD_PATH/loader.kip" --uncompress=hoc.kip
    cd "$ROOT_DIR" # exit
    cp -v build/atmosphere/stratosphere/loader/hoc.kip dist/atmosphere/kips/hoc.kip
fi

if [ "$NO_EXO" -eq 0 ]; then
    echo
    echo "*** Compiling exosphere ***"
    cd build/atmosphere/exosphere
    make -j$JOBS
    cd "$ROOT_DIR"
    cp -v build/atmosphere/exosphere/out/nintendo_nx_arm64_armv8a/release/exosphere.bin dist/atmosphere/exosphere.bin
fi

if [ -n "$ATMOSPHERE_CCACHE" ]; then
    echo
    echo "*** ccache stats ***"
    ccache --show-stats
fi

if should_skip clk; then
    echo
    echo "*** Skipping hoc-clk ***"
else
    cd Source/hoc-clk/ || exit 1
    if [ "$DEBUG_BUILD" -eq 1 ]; then
        ./build.sh "" -d
    else
        ./build.sh
    fi
    cp -r dist/ ../../

    cd "$ROOT_DIR"
fi

if should_skip sm; then
    echo
    echo "*** Skipping Status-Monitor ***"
else
    echo "*** Downloading Status-Monitor ***"
    wget "https://github.com/ppkantorski/Status-monitor-overlay/releases/latest/download/Status-Monitor-Overlay.ovl"
    mv -v Status-Monitor-Overlay.ovl "$DIST_DIR"/switch/.overlays/Status-Monitor-Overlay.ovl
fi


if [ "$EXT" -eq 1 ]; then
    echo
    echo "*** Compiling extensions ***"

    if should_skip hek; then
        echo
        echo "*** Skipping custom Hekate ***"
    else
        require_toolchain DEVKITARM arm-none-eabi-gcc base_rules "custom Hekate (devkitARM)"

        HEKATE_DIR="$ROOT_DIR/Source/hekate"
        HEKATE_URL="https://github.com/Horizon-OC/hekate.git"

        if [ ! -d "$HEKATE_DIR" ]; then
            echo
            echo "*** Cloning custom Hekate ***"
            git clone "$HEKATE_URL" "$HEKATE_DIR" || exit 1
        else
            echo
            echo "*** Updating custom Hekate ***"
            git -C "$HEKATE_DIR" pull --ff-only || exit 1
        fi

        cd "$HEKATE_DIR" || exit 1
        echo
        echo "*** Compiling custom Hekate ***"
        # Prevent crash on MSYS2
        make -j"$JOBS" SHELL=/usr/bin/bash || exit 1
        echo

        mkdir -p "$DIST_DIR/bootloader/sys/"
        cp -v output/nyx.bin "$DIST_DIR/bootloader/sys/nyx.bin"
        cp -v output/hekate.bin "$DIST_DIR/bootloader/update.bin"
        cp -v output/hekate.bin "$DIST_DIR/payload.bin"
    fi

    if should_skip btb; then
        echo
        echo "*** Skipping Benchmark-Toolbox ***"
    else
        cd "$ROOT_DIR/Source/Benchmark-Toolbox" || exit 1
        echo
        echo "*** Compiling Benchmark-Toolbox ***"
        make -j"$JOBS" || exit 1
        cp -v Benchmark-Toolbox.nro "$DIST_DIR/switch/Benchmark-Toolbox.nro"
    fi
fi

if should_skip zip; then
    echo
    echo "*** Skipping dist.zip packaging ***"
else
    echo
    echo "*** Packaging dist.zip ***"

    cd "$DIST_DIR" || exit 1

    rm -f dist.zip
    rm .gitignore

    zip -r dist.zip . >/dev/null

    echo "*** dist.zip created ***"
    echo

    touch .gitignore

    cd "$ROOT_DIR" || exit 1
fi
