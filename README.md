# PRTS Cursor

[English](README.md) | [简体中文](README.zh-CN.md)

Cursor theme and shader-effect switcher for [Umbriel](https://github.com/noctalia-dev/umbriel), built with [HuxerUI](https://github.com/HuxerUI/HuxerUI). Ships the PRTS cursor theme from [crowninggg/prts-cursor](https://github.com/crowninggg/prts-cursor).

![screenshot](docs/screenshot.png)

## Features

- Bundled theme: `PRTS-Final` (animated, ~697 MB unpacked / ~100 MB packed) and `PRTS-Final-Static`
- Switch Xcursor theme (PRTS animated / PRTS static / Bibata / Adwaita) and size (16–72 px)
- Auto-discovers community cursor shader effects; newly installed effects are added to Umbriel's `[include]` when selected
- Changes apply immediately — no Apply button
- System tray: show/hide window, reset to Niri defaults, quit
- Floating window with custom title bar

## Packages

RPM / DEB / AppImage / tar.zst are attached to [Releases](https://github.com/h465855hgg/prts-cursor/releases). All of them contain both PRTS themes.

```sh
sudo dnf install ./prts-cursor-1.2.0-1.fc45.x86_64.rpm
sudo apt install ./prts-cursor_1.2.0-1_amd64.deb
chmod +x prts-cursor-1.2.0-x86_64.AppImage && ./prts-cursor-1.2.0-x86_64.AppImage
```

## Notes

- The theme is fan-made art derived from Arknights; copyright belongs to the original rights holders. Redistribution is the user's own responsibility
- Effects are single-select; Umbriel can only mount one cursor effect at a time
- The generic, theme-independent version is [umbriel-cursor-switcher](https://github.com/h465855hgg/umbriel-cursor-switcher)

## License

MIT (the application only; the bundled theme keeps its own credits)
