# Package

version     = "0.1.0"
author      = "TurtleP"
description = "Nintendo Homebrew Binaries Updater"
license     = "MIT"
srcDir      = "src"
bin         = @["hbupdater"]
binDir      = "dist"

# Dependencies

requires "nim >= 1.6.4"
requires "cligen"
requires "https://github.com/TurtleP/nimtenbrew#HEAD"
requires "unpack"
requires "chain"
