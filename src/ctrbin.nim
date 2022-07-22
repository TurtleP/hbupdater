import nimtenbrew/ctr/binary
import nimtenbrew/ctr/smdh
import nimtenbrew/ctr/romfs

import unpack
import chain

import strings

import streams
import strutils
import tables
import os

const CtrParamHelp*: Table[string, string] =
    {"filepath": "path to the 3dsx file",
    "metadata": "path to the new SMDH file or title info [name] [description] [author]",
    "iconPath": "path to the new SMDH icon from a PNG",
    "romfsPath": "path to the new RomFS file",
    "output": "path to output the new 3dsx file, including filename"}.toTable()

proc getRelocationData(header: CtrHeader, stream: FileStream): (array[NUMBER_RELOC_TABLES, CtrRelocationHeader], uint) =

    let relocationHeaderSize = header.relocationHeaderSize

    var relocationHeaders: array[NUMBER_RELOC_TABLES, CtrRelocationHeader]
    var totalRelocations: uint = 0

    for index in 0 ..< NUMBER_RELOC_TABLES:
        let start: int64 = stream.getPosition()

        let buffer = stream.readStr(relocationHeaderSize.int)
        relocationHeaders[index] = toCtrRelocationHeader(buffer)

        let read = stream.getPosition() - start

        if (read != relocationHeaderSize.int64):
            strings.error(Error.CannotReadRelocHeader, index)

        totalRelocations += relocationHeaders[index].absolute
        totalRelocations += relocationHeaders[index].relative

    return (relocationHeaders, totalRelocations)

proc ctr*(filepath: string, metadata = newSeq[string](), iconPath = "", romfsPath: string = "",
        output: string) =
    ## Updates the binary for a Nintendo 3DS (*.3dsx) homebrew application

    let fileStream = streams.newFileStream(filepath)

    if (fileStream.isNil):
        strings.error(Error.FailedToOpen, filepath)

    # binarylang checks the magic value from the struct and will error if invalid
    let header = toCtrHeader(fileStream.readStr(BINARY_HEADER_SIZE.int))

    if (header.headerSize <= BINARY_HEADER_SIZE):
        strings.error(Error.NoExtendedHeader)

    let extendedHeader = toCtrExtendedHeader(fileStream.readStr(EXTENDED_HEADER_SIZE.int))

    let (relocations, totalRelocations) = getRelocationData(header, fileStream)

    var executionSize: uint = totalRelocations * RELOCATION_SIZE
    executionSize += (header.codeSegmentSize + header.rodataSegmentSize)
    executionSize += (header.dataSegmentSize - header.bssSize)

    let executionData = fileStream.readStr(executionSize.int)

    var smdhBinary: Smdh

    if (len(metadata) > 0x0):
        # check if the new smdh file exists
        if (os.fileExists(metadata[0])):
            smdhBinary = toSmdh(io.readFile(metadata[0]))

            extendedHeader.smdhOffset = fileStream.getPosition().uint32
            extendedHeader.smdhSize = os.getFileSize(metadata[0]).uint32
        else:
            smdhBinary = toSmdh(fileStream.readStr(SMDH_STRUCT_SIZE.int))

            # if not, try setting new metadata
            if (len(metadata) <= 0x03):
                metadata.unpackSeq(short, long, author)
                smdhBinary.setTitles(short, long, author)
            else:
                strings.error(Error.FileOrMetadataExpected)
    else:
        # grab the default smdh file data
        if extendedHeader.smdhSize != 0:
            smdhBinary = toSmdh(fileStream.readStr(SMDH_STRUCT_SIZE.int))

    if (not smdhBinary.isNil and not iconPath.isEmptyOrWhitespace()):
        smdhBinary.setIcon(iconPath)

    var romfsBuffer: string

    if (os.fileExists(romfsPath)):
        romfsBuffer = io.readFile(romfsPath)
        discard toRomfs(romfsBuffer)

        extendedHeader.romfsOffset = extendedHeader.smdhOffset + extendedHeader.smdhSize
    else:
        if extendedHeader.romfsOffset != 0:
            if fileStream.getPosition() == extendedHeader.smdhOffset.int:
                fileStream.setPosition(extendedHeader.romfsOffset.int)

            romfsBuffer = fileStream.readAll()

    chain(io.open(output, fmWrite) as updatedFile):
        write(header.fromCtrHeader())
        write(extendedHeader.fromCtrExtendedHeader())

        for index in 0 ..< NUMBER_RELOC_TABLES:
            write(relocations[index].fromCtrRelocationHeader())

        write(executionData)

        if extendedHeader.smdhOffset != 0:
            write(smdhBinary.fromSmdh())

        if extendedHeader.romfsOffset != 0:
            write(romfsBuffer)


    updatedFile.close()
