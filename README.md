# DLSS Indicator Toggle

NVIDIA DLSS has a hidden debug overlay that, when enabled, shows the active DLSS DLL version, render resolution, preset, DLSS-G frame generation ratio, Reflex status, and more in-game. This tool toggles it — essentially flipping one registry DWORD: `HKLM\SOFTWARE\NVIDIA Corporation\Global\NGXCore\ShowDlssIndicator`.

| Value | Effect |
|---|---|
| `0` / missing | Off |
| `1` | Green square (dev DLLs only) |
| `1024` | Full text overlay (SR/RR bottom-left + DLSS-G top) |

You can do this in regedit just as easily — this just saves creating the key by hand. 136 KB single exe, no dependencies.

## Usage

Download the exe from [Releases](../../releases), run it (requires admin, since it writes HKLM), restart your game.

## Build

Visual Studio 2022 with the C++ workload. Run `_build_msvc.bat`.

## License

MIT
