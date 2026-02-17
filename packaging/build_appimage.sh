#!/usr/bin/env bash
set -euo pipefail

# Folder where this script lives
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Project root = parent of packaging/  (adjust if your layout differs)
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

APP=sticky_note
APPDIR="AppDir"
BUILD_DIR="$ROOT_DIR/cmake-build-release"   # <-- CLion default
DISTDIR="$ROOT_DIR/dist"

LINUXDEPLOY="./tools/linuxdeploy-x86_64.AppImage"
PLUGIN_QT="./tools/linuxdeploy-plugin-qt-x86_64.AppImage"
APPIMAGETOOL="./tools/appimagetool-x86_64.AppImage"

# 1) Setup environment
cd "$ROOT_DIR"
mkdir -p tools

# Function to download tool if missing
download_tool() {
  local url=$1
  local output=$2
  if [ ! -f "$output" ]; then
    echo "Downloading $output..."
    curl -L "$url" -o "$output"
    chmod +x "$output"
  fi
}

download_tool "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" "$LINUXDEPLOY"
download_tool "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage" "$PLUGIN_QT"
download_tool "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage" "$APPIMAGETOOL"

# 2) Clean AppDir
APPDIR="$ROOT_DIR/StickyNoteApp"
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"

# 3) Copy your binary + desktop + icon
cp "$BUILD_DIR/$APP" "$APPDIR/usr/bin/"
cp "$ROOT_DIR/sticky_note.desktop" "$APPDIR/usr/share/applications/"
cp "$ROOT_DIR/icons/note_icon.png" "$APPDIR/usr/share/icons/hicolor/256x256/apps/sticky_note.png"

# 4) Bundle Qt dependencies into the AppDir
# linuxdeploy uses plugins; the Qt plugin bundles Qt libs/plugins/resources (Qt6 supported).
export QML_SOURCES_PATHS=""   # set this if you use QML; can leave empty for Widgets-only
export LINUXDEPLOY_PLUGINS="qt"
export LINUXDEPLOY_PLUGIN_QT_PATH="$PLUGIN_QT"

# Make sure tools can run (some environments need this for AppImages)
export APPIMAGE_EXTRACT_AND_RUN=1

"$LINUXDEPLOY" \
  --appdir "$APPDIR" \
  -e "$APPDIR/usr/bin/$APP" \
  -d "$APPDIR/usr/share/applications/sticky_note.desktop" \
  -i "$APPDIR/usr/share/icons/hicolor/256x256/apps/sticky_note.png"

# 5) AppRun entry point (required by AppImage spec)
# Many tools generate it, but ensure it exists.
if [ ! -e "$APPDIR/AppRun" ]; then
  ln -s "usr/bin/$APP" "$APPDIR/AppRun"
fi

# 6) Build the AppImage from AppDir
mkdir -p "$DISTDIR"
ARCH=x86_64 "$APPIMAGETOOL" "$APPDIR" "$DISTDIR/Sticky_Note-x86_64.AppImage"

echo "Done: $DISTDIR/Sticky_Note-x86_64.AppImage"
