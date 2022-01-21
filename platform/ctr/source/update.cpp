#include "update.hpp"

#include "3dsx.hpp"
#include "romfs.hpp"
#include "smdh.hpp"

#include "file.hpp"

#include <memory.h>
#include <stdio.h>
#include <sys/stat.h>

/**
 * @brief Update the SMDH and RomFS sections
 *
 * @param info Arguments from main
 * @return int success value
 */
int update::init(const args::Info& args)
{
    File executable(args.filepath);

    /* open the 3dsx for reading */
    if (!executable.Open(File::MODE_READ))
        return -1;

    /* read in the header data */
    _3DSX::Header header {};
    executable.Read(&header, _3DSX::HEADER_SIZE);

    /* verify the magic in the header */
    if (strncmp(header.magic, _3DSX::MAGIC, 4) != 0)
        error("Invalid 3DSX file.");

    if (header.headerSize <= 32)
        error("No extended header found.");

    _3DSX::ExtendedHeader extendedHeader {};
    executable.Read(&extendedHeader, _3DSX::EXT_HEADER_SIZE);

    /* get our relocation type count */
    uint32_t numRelocationTypes = header.relocationHeaderSize / 4;

    size_t numRelocations = 0;

    /* there are 3 relocation headers */
    _3DSX::RelocationHeader relocationHeaders[_3DSX::NUM_RELOC_TABLES];
    std::fill_n(relocationHeaders, _3DSX::NUM_RELOC_TABLES, _3DSX::RelocationHeader {});

    /* iterate our relocation table headers */
    for (size_t index = 0; index < _3DSX::NUM_RELOC_TABLES; index++)
    {
        int64_t read = executable.Read(&relocationHeaders[index], header.relocationHeaderSize);

        if (read != header.relocationHeaderSize)
            errorf("Cannot read relocation header %d", index);

        /* add up the total relocations */
        for (uint32_t type = 0; type < numRelocationTypes; type++)
        {
            uint32_t* buffer = reinterpret_cast<uint32_t*>(&relocationHeaders[index]);
            numRelocations += buffer[type];
        }
    }

    /* calculate the execution size */
    size_t executionSize = numRelocations * _3DSX::RELOCATION_SIZE;
    executionSize += header.codeSegmentSize + header.rodataSegmentSize;
    executionSize += (header.dataSegmentSize - header.bssSize);

    /* create a data object to hold the data */
    std::vector<uint8_t> executionData(executionSize);

    /* check if it's valid, read it in if so */
    executable.Read(executionData.data(), executionSize);

    /* get the SMDH file data */

    SMDH::Header smdhHeader {};
    std::vector<uint8_t> smdhData = update::read(args.smdhPath);

    if (!smdhData.empty())
    {
        /* validate the new data */
        memcpy(&smdhHeader, smdhData.data(), SMDH::HEADER_SIZE);

        if (strncmp(smdhHeader.magic, SMDH::MAGIC, 4) != 0)
            error("Invalid SMDH file.");

        /* set the extended header data for the SMDH */
        extendedHeader.smdhOffset = executable.Tell();
        extendedHeader.smdhSize   = smdhData.size();
    }
    else
    {
        /* if we didn't get any SMDH data, use the original data */
        if (extendedHeader.smdhSize != 0)
        {
            smdhData.resize(extendedHeader.smdhSize);
            executable.Read(smdhData.data(), smdhData.size());
        }
    }

    RomFS::Header romfsHeader {};
    std::vector<uint8_t> romfsData = update::read(args.romfsPath);

    if (!romfsData.empty())
    {
        /* validate the new RomFS data */
        memcpy(&romfsHeader, romfsData.data(), RomFS::HEADER_SIZE);

        if (strncmp(romfsHeader.magic, RomFS::MAGIC, 4) != 0)
            error("Invalid RomFS file.");

        /* set the extended header data for the RomFS */
        extendedHeader.romfsOffset = extendedHeader.smdhOffset + extendedHeader.smdhSize;
    }
    else
    {
        if (extendedHeader.romfsOffset != 0)
        {
            /* if we didn't get any RomFS data, use the original data */
            if (executable.Tell() == extendedHeader.smdhOffset)
                executable.Seek(extendedHeader.romfsOffset);

            romfsData = executable.Read();
        }
    }

    File updated(args.outPath);

    if (!updated.Open(File::MODE_WRITE))
        errorf("Failed to open %s for writing.", updated.GetFilename().c_str());

    /* write our header */
    updated.Write(&header, _3DSX::HEADER_SIZE);

    /* write our extended header */
    updated.Write(&extendedHeader, _3DSX::EXT_HEADER_SIZE);

    /* write our relocation headers */
    for (size_t index = 0; index < _3DSX::NUM_RELOC_TABLES; index++)
        updated.Write(&relocationHeaders[index], header.relocationHeaderSize);

    /* write the execution data */
    updated.Write(executionData.data(), executionData.size());

    /* write the SMDH data */
    if (smdhData.size() != 0)
        updated.Write(smdhData.data(), smdhData.size());

    /* write the RomFS data */
    if (romfsData.size() != 0)
        updated.Write(romfsData.data(), romfsData.size());

    if (!updated.Close())
        errorf("Failed to close file %s.", updated.GetFilename().c_str());

    return 0;
}
