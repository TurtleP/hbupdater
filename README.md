[![CI](https://github.com/TurtleP/nintenbrew-updaters/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/TurtleP/nintenbrew-updaters/actions/workflows/c-cpp.yml)

## Requirements
There aren't any real "requirements" but if you want to properly update the binary sections, you [will need `devkitpro-pacman`](https://devkitpro.org/wiki/devkitPro_pacman). Once installed, you will need the following packages installed to build certain things, e.g. metadata and RomFS.

- 3dstools
- switch-tools

## Current Supported Binaries
- Nintendo 3DS (3DSX)
- Nintendo Switch (NRO)

## Compilation
Simply running `make` will output `3dsxupdate` and `nroupdate` in the `dist` folder on root. If you want to only have one:
1. 3DSX: `make ctr`
2. NRO: `make hac`
