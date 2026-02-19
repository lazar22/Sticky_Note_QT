#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

APP=sticky_note
BUILD_DIR="$ROOT_DIR/cmake-build-release"
DISTDIR="$ROOT_DIR/dist"

# Output AppDir name (you used this in the zip step)
APPDIR="$ROOT_DIR/StickyNoteApp"

LINUXDEPLOY="$ROOT_DIR/tools/linuxdeploy-x86_64.AppImage"
PLUGIN_QT="$ROOT_DIR/tools/linuxdeploy-plugin-qt-x86_64.AppImage"
APPIMAGETOOL="$ROOT_DIR/tools/appimagetool-x86_64.AppImage"

mkdir -p "$ROOT_DIR/tools"

download_tool() {
  local url=$1
  local output=$2
  if [ ! -f "$output" ]; then
    echo "Downloading $output..."
    curl -L "$url" -o "$output"
    chmod +x "$output"
  fi
}

# Prefer pinning versions in the future; continuous works but can change.
download_tool "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" "$LINUXDEPLOY"
download_tool "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage" "$PLUGIN_QT"
download_tool "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage" "$APPIMAGETOOL"

# Clean AppDir
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"

# Copy your binary + desktop + icon
cp "$BUILD_DIR/$APP" "$APPDIR/usr/bin/"
cp "$ROOT_DIR/sticky_note.desktop" "$APPDIR/usr/share/applications/"
cp "$ROOT_DIR/icons/note_icon.png" "$APPDIR/usr/share/icons/hicolor/256x256/apps/sticky_note.png"

# Bundle Qt deps
export QML_SOURCES_PATHS=""
export LINUXDEPLOY_PLUGINS="qt"
export LINUXDEPLOY_PLUGIN_QT_PATH="$PLUGIN_QT"
export EXTRA_QT_PLUGINS="platforms,xcbglintegrations,platformthemes"
export APPIMAGE_EXTRACT_AND_RUN=1

"$LINUXDEPLOY" \
  --appdir "$APPDIR" \
  -e "$BUILD_DIR/$APP" \
  -d "$ROOT_DIR/sticky_note.desktop" \
  -i "$ROOT_DIR/icons/note_icon.png" \
  --plugin qt

# Create a real AppRun wrapper (so running AppDir directly works too)
cat > "$APPDIR/AppRun" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"

# Prefer bundled libs (Qt, stdc++, etc.) inside the AppDir.
export LD_LIBRARY_PATH="$HERE/usr/lib:$HERE/usr/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH:-}"

# Qt plugin + QML paths (safe even if not used)
export QT_PLUGIN_PATH="$HERE/usr/plugins"
export QML2_IMPORT_PATH="$HERE/usr/qml"

# Ensure desktop files/icons can be found
export XDG_DATA_DIRS="$HERE/usr/share:${XDG_DATA_DIRS:-/usr/local/share:/usr/share}"

exec "$HERE/usr/bin/sticky_note" "$@"
EOF
chmod +x "$APPDIR/AppRun"

# Sanity check: make sure Qt got bundled (otherwise runtime will use system Qt)
if [ ! -f "$APPDIR/usr/lib/libQt6Core.so.6" ] && [ ! -f "$APPDIR/usr/lib/x86_64-linux-gnu/libQt6Core.so.6" ]; then
  echo "ERROR: Qt6Core was not bundled into AppDir. linuxdeploy-plugin-qt likely didn't run correctly."
