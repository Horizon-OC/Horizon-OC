#!/bin/sh

EXT=0
LDR_MAKE="nx_release"
LDR_SET=0
NO_EXO=0
JOBS=""
DEBUG_BUILD=0

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

LDR_BUILD_PATH="${LDR_MAKE#nx_}"

echo
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
    LIBEXO_DEST="build/atmosphere/libraries/libexosphere/include/exosphere/secmon"

    cp -v "$EXO_SRC/secmon_emc_access_table_data.inc"       "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_define_emc_access_table.inc"     "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_rtc_pmc_access_table_data.inc"   "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_define_rtc_pmc_access_table.inc" "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_smc_register_access.cpp"         "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_smc_handler.cpp"                 "$EXO_DEST/"
    cp -v "$EXO_SRC/secmon_memory_layout.hpp"               "$LIBEXO_DEST/"
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

echo
echo "*** Compiling loader ***"
cd build/atmosphere/stratosphere/loader || exit 1
make -j$JOBS HOC_UART_LOG=$DEBUG_BUILD "$LDR_MAKE"
hactool -t kip1 "out/nintendo_nx_arm64_armv8a/$LDR_BUILD_PATH/loader.kip" --uncompress=hoc.kip
cd "$ROOT_DIR" # exit
cp -v build/atmosphere/stratosphere/loader/hoc.kip dist/atmosphere/kips/hoc.kip

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

cd Source/hoc-clk/
if [ "$DEBUG_BUILD" -eq 1 ]; then
    ./build.sh "" -d
else
    ./build.sh
fi
cp -r dist/ ../../

cd "$ROOT_DIR"

echo "*** Downloading Status-Monitor ***"
wget "https://github.com/ppkantorski/Status-monitor-overlay/releases/latest/download/Status-Monitor-Overlay.ovl"
mv -v Status-Monitor-Overlay.ovl "$DIST_DIR"/switch/.overlays/Status-Monitor-Overlay.ovl


if [ "$EXT" -eq 1 ]; then
    cd Source/

    echo
    echo "*** Compiling extensions ***"

    HEKATE_DIR="hekate"
    HEKATE_URL="https://github.com/Horizon-OC/hekate.git"

    if [ ! -d "$HEKATE_DIR" ]; then
        echo
        echo "*** Cloning custom Hekate ***"
        git clone "$HEKATE_URL" "$HEKATE_DIR"
    fi

    cd hekate/
    echo
    echo "*** Compiling custom Hekate ***"
    make -j$JOBS
    echo

    mkdir -p "$DIST_DIR/bootloader/sys/"
    cp -v output/nyx.bin "$ROOT_DIR"/dist/bootloader/sys/nyx.bin
    cp -v output/hekate.bin "$ROOT_DIR"/dist/bootloader/update.bin
    cp -v output/hekate.bin "$ROOT_DIR"/dist/payload.bin

    cd "$ROOT_DIR"/Source/Benchmark-Toolbox
    echo
    echo "*** Compiling Benchmark-Toolbox ***"
    make -j$JOBS
    cp -v Benchmark-Toolbox.nro "$DIST_DIR"/switch/Benchmark-Toolbox.nro
fi

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
