#pragma once

#include <stdint.h>

namespace RomFS
{
    static constexpr char MAGIC[4]         = { 'I', 'V', 'F', 'C' };
    static constexpr uint32_t MAGIC_NUMBER = 0x10000;

    /**
     * @brief RomFS Header
     * https://www.3dbrew.org/wiki/RomFS#Format
     */
    struct Header
    {
        char magic[4];
        uint32_t magicNumber;

        uint32_t masterHashSize;

        uint64_t levelOneLogicOffset;
        uint64_t levelOneHashSize;
        uint32_t levelOneLog2BlockSize;

        uint32_t reserved;

        uint64_t levelTwoLogicOffset;
        uint64_t levelTwoHashSize;
        uint32_t levelTwoLog2BlockSize;

        uint32_t reservedTwo;

        uint64_t levelThreeLogicOffset;
        uint64_t levelThreeHashSize;
        uint32_t levelThreeLog2BlockSize;

        uint32_t reservedThree;
        uint32_t reservedFour;

        uint32_t size;
    };

    static constexpr size_t HEADER_SIZE = sizeof(Header);
} // namespace RomFS
