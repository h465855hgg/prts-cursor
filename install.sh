#!/bin/sh
# 构建并安装到 ~/.local/opt/prts-cursor
set -eu
cd "$(dirname "$0")"

HUXERUI_HOME=${HUXERUI_HOME:-$HOME/.local/share/HuxerUI}
export HUXERUI_HOME
PATH="$HUXERUI_HOME/bin:$PATH"

huxerui build linux --config release

BUILD=$(find .huxerui/build -type f -name prts_cursor_hui -perm -u+x -path '*release*' | head -1)
[ -n "$BUILD" ] || { echo "构建产物没找到" >&2; exit 1; }
RES="$(dirname "$BUILD")/prts_cursor_hui.resources"

DEST="$HOME/.local/opt/prts-cursor"
mkdir -p "$DEST"
install -m755 "$BUILD" "$DEST/prts-cursor"
rm -rf "$DEST/prts-cursor.resources"
cp -r "$RES" "$DEST/prts-cursor.resources"
install -m644 helper.py icon.png "$DEST/"
install -m644 prts-cursor.desktop "$HOME/.local/share/applications/prts-cursor.desktop"
install -Dm644 icon.png "$HOME/.local/share/icons/hicolor/128x128/apps/prts-cursor.png"
update-desktop-database "$HOME/.local/share/applications" 2>/dev/null || true
gtk-update-icon-cache -f -t "$HOME/.local/share/icons/hicolor" 2>/dev/null || true
echo "已安装：$DEST/prts-cursor"
