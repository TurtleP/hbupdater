import nimtenbrew/hac/binary
import nimtenbrew/hac/assets
import nimtenbrew/hac/nacp

import strings

import chain

import os
import streams
import tables

const HacParamHelp*: Table[string, string] =
    {"filepath": "path to the nro file",
    "metadata": "path to the new NACP file",
    "title": "title of the application",
    "author": "creator(s) of the application",
    "iconPath": "path to the new icon from a JPG",
    "romfsPath": "path to the new RomFS file",
    "output": "path to output the new nro file, including filename"}.toTable()

proc hac*(filepath: string, metadata = "", title = "", author = "",
        iconPath = "", romfsPath = "", output: string) =
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
    var assetsHeader: AssetsHeader

    try:
        assetsHeader = toAssetsHeader(fileStream.readStr(ASSETS_HEADER_SIZE.int))
    except:
        assetsHeader = AssetsHeader()

    # set the initial offset for the assets
    var assetsOffset: uint64 = ASSETS_HEADER_SIZE

    # handle the icon
    var iconBuffer: string

    if (os.fileExists(iconPath)):
        iconBuffer = io.readFile(iconPath)

        assetsHeader.icon.offset = assetsOffset
        assetsHeader.icon.size = os.getFileSize(iconPath).uint32
    else:
        if (assetsHeader.icon.size != 0):
            # capture original icon offset
            var iconOffset: uint64 = assetsHeader.icon.offset

            # read original data from original position
            fileStream.setPosition((header.totalSize + iconOffset).int)
            iconBuffer = fileStream.readStr(assetsHeader.icon.size.int)

    assetsOffset += assetsHeader.icon.size

    # handle the nacp vs metadata situation
    var nacpBinary: Nacp

    if (os.fileExists(metadata)):
        nacpBinary = toNacp(io.readFile(metadata))

        assetsHeader.nacp.offset = assetsOffset
        assetsHeader.nacp.size = os.getFileSize(metadata).uint32
    else:
        if (assetsHeader.nacp.size != 0):
            # capture original nacp offset
            var nacpOffset: uint64 = assetsHeader.nacp.offset

            # if the nacp offset isn't our cpatured offset, then update it
            if (assetsOffset != nacpOffset):
                assetsHeader.nacp.offset = assetsOffset

             # read original data from original position
            fileStream.setPosition((header.totalSize + nacpOffset).int)
            nacpBinary = toNacp(fileStream.readStr(NACP_STRUCT_SIZE.int))

    if (not nacpBinary.isNil):
        # handled in nimtenbrew when these are empty
        nacpBinary.setTitles(title, author)

    assetsOffset += assetsHeader.nacp.size

    # handle the RomFS
    var romfsBuffer: string

    if (os.fileExists(romfsPath)):
        romfsBuffer = io.readFile(romfsPath)

        assetsHeader.romfs.offset = assetsOffset
        assetsHeader.romfs.size = os.getFileSize(romfsPath).uint32
    else:
        if (assetsHeader.romfs.size != 0):
            # capture original romfs offset
            var romfsOffset: uint64 = assetsHeader.romfs.offset

            # if the romfs offset isn't our cpatured offset, then update it
            if (assetsOffset != romfsOffset):
                assetsHeader.romfs.offset = assetsOffset

            # read original data from original position
            fileStream.setPosition((header.totalSize + romfsOffset).int)
            romfsBuffer = fileStream.readStr(assetsHeader.romfs.size.int)

    os.createDir(os.parentDir(output))

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
