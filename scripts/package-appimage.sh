#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
VERSION=1.0.0
ARCH=${ARCH:-$(uname -m)}
case "$ARCH" in aarch64|arm64) ARCH=aarch64 ;; esac
TOOLS=$PWD/tools
mkdir -p "$TOOLS"
fetch() {
  [ -x "$TOOLS/$2" ] && return 0
  curl -fsSL "$1" -o "$TOOLS/$2" && chmod +x "$TOOLS/$2"
}
fetch "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-${ARCH}.AppImage" "linuxdeploy-${ARCH}.AppImage"
fetch "https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-${ARCH}.AppImage" "appimagetool-${ARCH}.AppImage"
fetch "https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gtk/master/linuxdeploy-plugin-gtk.sh" "linuxdeploy-plugin-gtk.sh"
fetch "https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-${ARCH}" "runtime-${ARCH}"
APPDIR=$PWD/AppDir
BIN=$PWD/build-stage/opt/prts-cursor
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/share/applications" "$APPDIR/usr/share/icons/hicolor/128x128/apps" "$APPDIR/usr/share/metainfo"
cp -a "$BIN/prts-cursor" "$BIN/prts-cursor.resources" "$BIN/helper.py" "$BIN/icon.png" "$APPDIR/usr/bin/"
cp prts-cursor.desktop "$APPDIR/usr/share/applications/"
cp prts-cursor.metainfo.xml "$APPDIR/usr/share/metainfo/"
cp icon.png "$APPDIR/prts-cursor.png"
cp icon.png "$APPDIR/usr/share/icons/hicolor/128x128/apps/prts-cursor.png"
cat > "$APPDIR/AppRun" <<'RUN'
#!/bin/sh
exec "$APPDIR/usr/bin/prts-cursor" "$@"
RUN
chmod +x "$APPDIR/AppRun"
export PATH="$TOOLS:$PATH"
export ARCH
"$TOOLS/linuxdeploy-${ARCH}.AppImage" --appimage-extract-and-run \
  --appdir "$APPDIR" \
  --executable "$APPDIR/usr/bin/prts-cursor" \
  --desktop-file "$APPDIR/usr/share/applications/prts-cursor.desktop" \
  --icon-file icon.png \
  --plugin gtk
mkdir -p dist
rm -f "dist/prts-cursor-${VERSION}-${ARCH}.AppImage"
"$TOOLS/appimagetool-${ARCH}.AppImage" --appimage-extract-and-run --runtime-file "$TOOLS/runtime-${ARCH}" "$APPDIR" "dist/prts-cursor-${VERSION}-${ARCH}.AppImage"
ls -lh "dist/prts-cursor-${VERSION}-${ARCH}.AppImage"
