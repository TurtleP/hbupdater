import strutils
import strformat

const NimblePkgVersion* {.strdefine.} = ""

type
    Error* = enum
        # Generic Errors
        InvalidRomFSFile = "Invalid RomFS file `$1`."
        InvalidExecutionSize = "Invalid Execution Size! Expected $1, got $2."
        InvalidIconData = "Invalid icon data!"

        FailedToOpen = "Failed to open `$1` for reading."
        FailedToWrite = "Failed to open `$1` for writing."
        FailedToClose = "Failed to close file `$1`."

        FileOrMetadataExpected = "SMDH or Metadata was expected."
        NoMetadata = "No Metadata was found."

        # HAC Errors
        NoMetadataFound = "No $1 in AssetsHeader found."

        # CTR Errors
        InvalidCtrBinary = "Invalid 3DSX file."
        NoExtendedHeader = "No Extended Header was found."
        CannotReadRelocHeader = "Cannot read relocation header #$1"
        InvalidCtrSmdh = "Invalid SMDH file `$1`."
        InvalidCtrIconSize = "Invalid icon size: $1x$2, expected $3x$3"

proc error*(error: Error, args: varargs[string, `$`]) =
    raise newException(Exception, fmt("Error: {($error).format(args)} Aborting."))
