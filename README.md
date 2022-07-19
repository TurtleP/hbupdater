[![CI](https://github.com/TurtleP/nintenbrew-updaters/actions/workflows/CI.yml/badge.svg)](https://github.com/TurtleP/nintenbrew-updaters/actions/workflows/CI.yml)

## Requirements

The main requirements are `3dstools` and `switch-tools`, which can be obtained from [`devkitpro-pacman`](https://devkitpro.org/wiki/devkitPro_pacman). These have tools which can build respective metadata and RomFS images needed. However, you can also set the individual title/description/author information yourself instead.

## Usage

```
  hbupdater {SUBCMD}  [sub-command options & parameters]
where {SUBCMD} is one of:
  help  print comprehensive or per-cmd help
  ctr   Updates the binary for a Nintendo 3DS (*.3dsx) homebrew application
  hac   Updates the binary for a Nintendo Switch (*.nro) homebrew application

hbupdater {-h|--help} or with no args at all prints this message.
hbupdater --help-syntax gives general cligen syntax help.
Run "hbupdater {help SUBCMD|SUBCMD --help}" to see help for just SUBCMD.
Run "hbupdater help" to get *comprehensive* help.
Top-level --version also available
```

## Current Supported Binaries

- Nintendo 3DS (\*.3dsx)
- Nintendo Switch (\*.nro)

## Compilation

You will need to install [nim-lang](https://nim-lang.org/), although preferrably via [choosenim](https://github.com/dom96/choosenim). Once done, change to the working directory of this project and run `nimble build`.
