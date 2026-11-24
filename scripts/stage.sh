#!/bin/sh
# 把构建产物整理成统一的安装树：build-stage/
set -eu
cd "$(dirname "$0")/.."
BIN=$(find .huxerui/build -type f -name prts_cursor_hui -perm -u+x -path '*release*' | head -1)
[ -n "$BIN" ] || { echo "找不到构建产物，先跑 huxerui build linux --config release" >&2; exit 1; }
RES="$(dirname "$BIN")/prts_cursor_hui.resources"
rm -rf build-stage
mkdir -p build-stage/opt/prts-cursor build-stage/usr/bin build-stage/usr/share/applications build-stage/usr/share/icons/hicolor/128x128/apps build-stage/usr/share/metainfo
cp "$BIN" build-stage/opt/prts-cursor/prts-cursor
cp -r "$RES" build-stage/opt/prts-cursor/prts-cursor.resources
cp helper.py icon.png build-stage/opt/prts-cursor/
ln -sf ../opt/prts-cursor/prts-cursor build-stage/usr/bin/prts-cursor
sed 's#^Exec=.*#Exec=prts-cursor#; s#^Icon=.*#Icon=prts-cursor#' prts-cursor.desktop > build-stage/usr/share/applications/prts-cursor.desktop
cp icon.png build-stage/usr/share/icons/hicolor/128x128/apps/prts-cursor.png
cp prts-cursor.metainfo.xml build-stage/usr/share/metainfo/
echo "staged: $(du -sh build-stage | cut -f1)"
