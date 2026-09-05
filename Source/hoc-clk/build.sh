#!/bin/bash
set -e

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DIST_DIR="$ROOT_DIR/dist"
CORES="$(nproc --all)"
OVERLAY_DEBUG=0

for arg in "$@"; do
    case "$arg" in
        -d|--debug)
            OVERLAY_DEBUG=1
            ;;
        "")
            ;;
        *)
            DIST_DIR="$arg"
            ;;
    esac
done

echo
echo "*** Compiling bpmpfw ***"
pushd "$ROOT_DIR/bpmpfw"
make -j$CORES
popd > /dev/null

mkdir -p "$ROOT_DIR/sysmodule/data"
cp -vf "$ROOT_DIR/bpmpfw/out/bpmpfw.bin" "$ROOT_DIR/sysmodule/data/bpmpfw.bin"
echo

echo "*** Compiling hoc-clk ***"
TITLE_ID="$(grep -oP '"title_id":\s*"0x\K(\w+)' "$ROOT_DIR/sysmodule/perms.json")"

pushd "$ROOT_DIR/sysmodule"
make -j$CORES
popd > /dev/null

mkdir -p "$DIST_DIR/atmosphere/contents/$TITLE_ID/flags"
cp -vf "$ROOT_DIR/sysmodule/out/hoc-clk.nsp" "$DIST_DIR/atmosphere/contents/$TITLE_ID/exefs.nsp"
>"$DIST_DIR/atmosphere/contents/$TITLE_ID/flags/boot2.flag"
cp -vf "$ROOT_DIR/sysmodule/toolbox.json" "$DIST_DIR/atmosphere/contents/$TITLE_ID/toolbox.json"

echo
echo "*** Compiling hoc-clk-overlay ***"
pushd "$ROOT_DIR/overlay"
make -j$CORES OVERLAY_DEBUG=$OVERLAY_DEBUG
popd > /dev/null

mkdir -p "$DIST_DIR/switch/.overlays"
cp -vf "$ROOT_DIR/overlay/out/horizon-oc-overlay.ovl" "$DIST_DIR/switch/.overlays/horizon-oc-overlay.ovl"
echo

echo "*** Copying assets ***"
mkdir -p "$DIST_DIR/config/horizon-oc"
cp -vf "$ROOT_DIR/templates/settings.ini.template" "$DIST_DIR/config/horizon-oc/settings.ini.template"
mkdir -p "$DIST_DIR/config/horizon-oc/kip"
cp -vf "$ROOT_DIR/templates/kip/current.ini.template" "$DIST_DIR/config/horizon-oc/kip/current.ini.template"
mkdir -p "$DIST_DIR/config/horizon-oc/profiles"
cp -vf "$ROOT_DIR/templates/profiles/README.ini" "$DIST_DIR/config/horizon-oc/profiles/README.ini"
mkdir -p "$DIST_DIR/config/ultrahand/assets/notifications"
cp -vf  "$ROOT_DIR/assets/hoc.rgba" "$DIST_DIR/config/ultrahand/assets/notifications/hoc.rgba"

echo
echo "*** Copying lang ***"
mkdir -p "$DIST_DIR/config/horizon-oc/lang"
cp -vr "$ROOT_DIR/overlay/lang/." "$DIST_DIR/config/horizon-oc/lang/"
echo
