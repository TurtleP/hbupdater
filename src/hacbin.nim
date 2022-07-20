import nimtenbrew/hac/binary
import nimtenbrew/hac/assets
import nimtenbrew/hac/nacp

import strings

import chain
import unpack

import os
import streams
import strutils
import tables

const HacParamHelp*: Table[string, string] =
    {"filepath": "path to the nro file",
    "metadata": "path to the new NACP file or title info [name] [author]",
    "iconPath": "path to the new icon from a JPG",
    "romfsPath": "path to the new RomFS file",
    "output": "path to output the new nro file, including filename"}.toTable()

proc hac*(filepath: string, metadata = newSeq[string](), iconPath = "",
        romfsPath = "", output: string) =
    ## Updates the binary for a Nintendo Switch (*.nro) homebrew application

    let fileStream = streams.newFileStream(filepath)

    if (fileStream.isNil):
        strings.error(Error.FailedToOpen, filepath)

    # read the start portion of the nro file
    discard fileStream.readStr(START_SIZE.int)

    # read the header portion of the nro file
    let header = toHacHeader(fileStream.readStr(NRO_HEADER_SIZE.int))

    # reset back to the file start
    fileStream.setPosition(0)
    let executionData = fileStream.readStr(header.totalSize.int)

    # read the asset sections
    var assetsHeader = toAssetsHeader(fileStream.readStr(ASSETS_HEADER_SIZE.int))

    # set the initial offset for the assets
    var assetsOffset: uint64 = ASSETS_HEADER_SIZE

    # handle the icon
    var iconBuffer: string

    if (not iconPath.isEmptyOrWhitespace()):
        iconBuffer = io.readFile(iconPath)

        assetsHeader.icon.size = os.getFileSize(iconPath).uint32
    else:
        if (assetsHeader.icon.size != 0):
            fileStream.setPosition((header.totalSize + assetsHeader.icon.offset).int)
            iconBuffer = fileStream.readStr(assetsHeader.icon.size.int)

    assetsOffset += assetsHeader.icon.size

    # handle the nacp vs metadata situation
    var nacpBinary: Nacp

    if (len(metadata) > 0x0):
        # check if the new nacp file exists
        if (os.fileExists(metadata[0])):
            nacpBinary = toNacp(io.readFile(metadata[0]))

            assetsHeader.nacp.offset = assetsOffset
            assetsHeader.nacp.size = os.getFileSize(metadata[0]).uint32
        else:
            if (assetsHeader.nacp.size != 0):
                fileStream.setPosition((header.totalSize + assetsHeader.nacp.offset).int)
                nacpBinary = toNacp(fileStream.readStr(NACP_STRUCT_SIZE.int))

                # if not, try setting new metadata
                if (len(metadata) <= 0x02):
                    metadata.unpackSeq(name, author)
                    nacpBinary.setTitles(name, author)
                else:
                    strings.error(Error.NoMetadata)
    else:
        strings.error(Error.FileOrMetadataExpected)

    assetsOffset += assetsHeader.nacp.size

    # handle the RomFS
    var romfsBuffer: string

    if (not romfsPath.isEmptyOrWhitespace()):
        romfsBuffer = io.readFile(iconPath)

        assetsHeader.romfs.offset = assetsOffset
        assetsHeader.romfs.size = os.getFileSize(romfsPath).uint32
    else:
        if (assetsHeader.romfs.size != 0):
            fileStream.setPosition((header.totalSize + assetsHeader.romfs.offset).int)
            iconBuffer = fileStream.readStr(assetsHeader.romfs.size.int)

    chain(newFileStream(output, fmWrite) as updatedFile):
        write(executionData)
        write(assetsHeader.fromAssetsHeader())

        if (assetsHeader.icon.size != 0):
            setPosition((header.totalSize + assetsHeader.icon.offset).int)
            write(iconBuffer)

        if (assetsHeader.nacp.size != 0):
            setPosition((header.totalSize + assetsHeader.nacp.offset).int)
            write(nacpBinary.fromNacp())

        if (assetsHeader.romfs.size != 0):
            setPosition((header.totalSize + assetsHeader.romfs.offset).int)
            write(romfsBuffer)

    updatedFile.close()
