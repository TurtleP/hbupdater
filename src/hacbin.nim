import tables

const HacParamHelp*: Table[string, string] =
    {"filepath": "path to the nro file",
    "metadata": "path to the new NACP file or title info [name] [author]",
    "iconPath": "path to the new icon from a JPG",
    "romfsPath": "path to the new RomFS file",
    "output": "path to output the new nro file, including filename"}.toTable()

proc hac*(filepath: string, metadata = "", icon_path = "", romfs_path = "", output: string) =
    ## Updates the binary for a Nintendo Switch (*.nro) homebrew application
    echo("!")
