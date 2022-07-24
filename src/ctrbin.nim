import nimtenbrew/ctr/binary
import nimtenbrew/ctr/smdh

import chain

import strings

import streams
import strutils
import tables
import os

const CtrParamHelp*: Table[string, string] =
    {"filepath": "path to the 3dsx file",
    "metadata": "path to the new SMDH file",
    "title": "title of the application",
    "description": "description of the application",
    "author": "creator(s) of the application",
    "iconPath": "path to the new SMDH icon from a PNG",
    "romfsPath": "path to the new RomFS file",
    "output": "path to output the new 3dsx file, including filename"}.toTable()

proc getRelocationData(header: CtrHeader, stream: FileStream): (array[
        NUMBER_RELOC_TABLES, CtrRelocationHeader], uint) =

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

proc ctr*(filepath: string, metadata = "", title = "", description = "",
        author = "", iconPath = "", romfsPath = "", output: string) =
    ## Updates the binary for a Nintendo 3DS (*.3dsx) homebrew application

    let fileStream = streams.newFileStream(filepath)

    if (fileStream.isNil):
        strings.error(Error.FailedToOpen, filepath)

    # binarylang checks the magic value from the struct and will error if invalid
    let header = toCtrHeader(fileStream.readStr(BINARY_HEADER_SIZE.int))

    var extendedHeader: CtrExtendedHeader

    # read the original extheader if it exists
    if (header.headerSize > BINARY_HEADER_SIZE):
        extendedHeader = toCtrExtendedHeader(fileStream.readStr(EXTENDED_HEADER_SIZE.int))
    else:
        # create a new extheader otherwise
        header.headerSize = (BINARY_HEADER_SIZE + EXTENDED_HEADER_SIZE).uint16
        extendedHeader = CtrExtendedHeader()

    let (relocations, totalRelocations) = getRelocationData(header, fileStream)

    var executionSize: uint = totalRelocations * RELOCATION_SIZE
    executionSize += (header.codeSegmentSize + header.rodataSegmentSize)
    executionSize += (header.dataSegmentSize - header.bssSize)

    let executionData = fileStream.readStr(executionSize.int)

    var smdhBinary: Smdh

    # check if the new smdh file exists
    if (os.fileExists(metadata)):
        smdhBinary = toSmdh(io.readFile(metadata))
        var smdhOffset: uint32 = fileStream.getPosition().uint32

        # if we're adding an extended header
        if (extendedHeader.smdhSize == 0):
            let relocationHeadersSize = (NUMBER_RELOC_TABLES * RELOCATION_HEADER_SIZE)
            smdhOffset = (header.headerSize.uint32 + relocationHeadersSize.uint32 + executionSize.uint32)

        extendedHeader.smdhOffset = smdhOffset
        extendedHeader.smdhSize = os.getFileSize(metadata).uint32
    else:
        # if the extheader didn't exist, this would be zero
        if (extendedHeader.smdhSize != 0):
            smdhBinary = toSmdh(fileStream.readStr(SMDH_STRUCT_SIZE.int))

    if (not smdhBinary.isNil):
        # handled in nimtenbrew when these are empty
        smdhBinary.setTitles(title, description, author)

        if (not iconPath.isEmptyOrWhitespace()):
            smdhBinary.setIcon(iconPath)

    var romfsBuffer: string

    if (os.fileExists(romfsPath)):
        romfsBuffer = io.readFile(romfsPath)

        extendedHeader.romfsOffset = extendedHeader.smdhOffset + extendedHeader.smdhSize
    else:
        if extendedHeader.romfsOffset != 0:
            if (fileStream.getPosition() == extendedHeader.smdhOffset.int):
                fileStream.setPosition(extendedHeader.romfsOffset.int)

            romfsBuffer = fileStream.readAll()

    os.createDir(os.parentDir(output))

    chain(io.open(output, fmWrite) as updatedFile):
        write(header.fromCtrHeader())
        write(extendedHeader.fromCtrExtendedHeader())

        for index in 0 ..< NUMBER_RELOC_TABLES:
            write(relocations[index].fromCtrRelocationHeader())

        write(executionData)

        if (extendedHeader.smdhOffset != 0):
            write(smdhBinary.fromSmdh())

        if (extendedHeader.romfsOffset != 0):
            write(romfsBuffer)


    updatedFile.close()
