#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace _3DSX
{
    static constexpr uint32_t MAGIC       = 0x58534433;
    static constexpr int NUM_RELOC_TABLES = 0x03;

    /**
     * @brief 3DSX Header
     * https://www.3dbrew.org/wiki/3DSX_Format#Header
     */
    struct Header
    {
        uint32_t magic;
        uint16_t headerSize;
        uint16_t relocationHeaderSize;
        uint32_t formatVersion;
        uint32_t flags;

        uint32_t codeSegmentSize;
        uint32_t rodataSegmentSize;
        uint32_t dataSegmentSize;
        uint32_t bssSize;
    };

    static constexpr size_t HEADER_SIZE = sizeof(Header);
    static_assert(HEADER_SIZE == 0x20);

    /**
     * @brief 3DSX Extended Header
     * https://www.3dbrew.org/wiki/3DSX_Format#Extended_Header
     */
    struct ExtendedHeader
    {
        uint32_t smdhOffset;
        uint32_t smdhSize;
        uint32_t romfsOffset;
    };

    static constexpr size_t EXT_HEADER_SIZE = sizeof(ExtendedHeader);
    static_assert(EXT_HEADER_SIZE == 0x0C);

    /**
     * @brief 3DSX Relocation Header
     * https://www.3dbrew.org/wiki/3DSX_Format#Relocation_Header
     */
    struct RelocationHeader
    {
        uint32_t cAbsolute;
        uint32_t cRelative;
    };

    static constexpr size_t RELOCATION_HEADER_SIZE = sizeof(RelocationHeader);
    static_assert(RELOCATION_HEADER_SIZE == 0x08);

    /**
     * @brief 3DSX Relocation Types
     * https://www.3dbrew.org/wiki/3DSX_Format#Relocation
     */
    struct Relocation
    {
        uint16_t skip;
        uint16_t patch;
    };

    static constexpr size_t RELOCATION_SIZE = sizeof(Relocation);
    static_assert(RELOCATION_SIZE == 0x04);
} // namespace _3DSX
