#include "update.hpp"

#include <filesystem>
#include <memory.h>
#include <stdio.h>
#include <sys/stat.h>

/**
 * @brief Update the SMDH and RomFS sections
 *
 * @param info Arguments from main
 * @return int success value
 */
int update::init(const args::Info& info)
{
    /* open the 3dsx for reading */
    FILE* file = fopen(info.filepath, "rb");

    if (!file)
    {
        printf("File %s does not exist.", info.filepath);
        return fclose(file);
    }

    /* get the header size and make new a new data buffer for it */
    size_t structHeaderSize = sizeof(_3DSX::Header);
    uint8_t* headerData     = new uint8_t[structHeaderSize];

    /* read the file into the header data buffer */
    fread(headerData, 1, structHeaderSize, file);

    _3DSX::Header header {};

    /* copy the header data buffer to the struct */
    bool hasExtendedHeader = false;
    memcpy(&header, headerData, sizeof(_3DSX::Header));

    /* make sure we have an extended header to work with */
    if (header.headerSize > 32)
        hasExtendedHeader = true;

    if (hasExtendedHeader)
    {
        /* create the new buffer for extended header data */
        _3DSX::ExtendedHeader extendedHeader {};
        uint8_t* extendedHeaderData = new uint8_t[header.headerSize - structHeaderSize];

        /* read the data into the struct */
        fread(extendedHeaderData, 1, header.headerSize - structHeaderSize, file);
        memcpy(&extendedHeader, extendedHeaderData, sizeof(_3DSX::ExtendedHeader));

        /* header.relocationHeaderSize divided by four */
        size_t numRelocationTables = header.relocationHeaderSize / 4;

        /* read our relocation headers */
        size_t numRelocations          = 0;
        uint32_t* relocationHeaderData = new uint32_t[3 * numRelocationTables];

        for (size_t relocationIndex = 0; relocationIndex < 3; relocationIndex++)
        {
            size_t relocationPosition = numRelocationTables * relocationIndex;
            size_t read               = fread(relocationHeaderData + relocationPosition, 1,
                                              header.relocationHeaderSize, file);

            if (read != header.relocationHeaderSize)
            {
                printf("Cannot read relocation header %d", relocationIndex);
                return -1;
            }

            for (uint32_t index = 0; index < numRelocationTables; index++)
                numRelocations += (relocationHeaderData + relocationPosition)[index];
        }

        /* calculate the execution section size */
        size_t executionSize = header.headerSize;
        executionSize += numRelocations * sizeof(_3DSX::Relocation);
        executionSize += header.codeSegmentSize + header.rodataSegmentSize;
        executionSize += (header.dataSegmentSize - header.bssSize);
        executionSize += (numRelocationTables * 3);

        /* create and read into a new buffer for the execution data */
        uint8_t* executionData = new uint8_t[executionSize];
        fread(executionData, 1, executionSize, file);

        /* read the new SMDH file and update fields accordingly */
        uint32_t smdhSize   = 0;
        uint8_t* smdhBuffer = update::read(info.smdhPath, smdhSize);
        if (smdhBuffer == nullptr)
            return -2;

        SMDH::Header smdhHeader {};
        memcpy(&smdhHeader, smdhBuffer, sizeof(SMDH::Header));

        if (smdhHeader.magic != SMDH::MAGIC)
            printf("Invalid SMDH file.");

        extendedHeader.smdhSize   = smdhSize;
        extendedHeader.smdhOffset = ftell(file);

        uint32_t romfsSize = 0;

        uint8_t* romfsBuffer = update::read(info.romfsPath, romfsSize);
        if (romfsBuffer == nullptr)
            return -2;

        extendedHeader.romfsOffset = extendedHeader.smdhOffset + extendedHeader.smdhSize;

        size_t finalSize = header.headerSize + (numRelocationTables * 3);
        finalSize += executionSize + smdhSize + romfsSize;

        uint8_t* outData = new uint8_t[finalSize];

        /* copy the header */
        memcpy(outData, headerData, structHeaderSize);

        /* copy the extended header */
        size_t extendedHeaderSize = header.headerSize - structHeaderSize;
        memcpy(outData + structHeaderSize, extendedHeaderData, extendedHeaderSize);

        /* copy the relocation header (comes right after extended header) */
        /* header.headerSize includes the extended header size */
        memcpy(outData + header.headerSize, relocationHeaderData, (numRelocationTables * 3));

        /* copy the execution data (rodata relocation header -> data relocation table) */
        memcpy(outData + header.headerSize + (numRelocationTables * 3), executionData,
               executionSize);

        /* copy the SMDH data */
        memcpy(outData + extendedHeader.smdhOffset, smdhBuffer, smdhSize);

        /* copy the RomFS data */
        memcpy(outData + extendedHeader.romfsOffset, romfsBuffer, romfsSize);

        /* write the final output */
        FILE* outFile = fopen(info.outPath, "wb");
        fwrite(outData, 1, finalSize, outFile);

        /* cleanup */

        delete[] romfsBuffer;

        delete[] smdhBuffer;

        delete[] executionData;

        delete[] relocationHeaderData;

        delete[] extendedHeaderData;
    }
    else /* no extended header, return -1 */
    {
        update::exit(file, headerData);

        return -1;
    }

    update::exit(file, headerData);

    return 0;
}

void update::exit(FILE* file, uint8_t* data)
{
    if (data)
        delete[] data;

    if (file)
        fclose(file);
}

/**
 * @brief Read a file
 *
 * @param filepath Path to the file to read
 * @param size output size of the file
 * @return uint8_t* Pointer to the buffer
 */
uint8_t* update::read(char* filepath, uint32_t& size)
{
    FILE* file = fopen(filepath, "rb");

    if (!file)
    {
        printf("File %s does not exist.", filepath);
        fclose(file);

        return nullptr;
    }

    struct stat fileStat;
    stat(filepath, &fileStat);

    uint8_t* data = new uint8_t[fileStat.st_size];
    fread(data, 1, fileStat.st_size, file);

    size = fileStat.st_size;

    return data;
}
