#pragma once

#include "types.hpp"

namespace _3DSX
{
    static constexpr int MAGIC = 0x58534433;

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

    struct ExtendedHeader
    {
        uint32_t smdhOffset;
        uint32_t smdhSize;
        uint32_t romfsOffset;
    };

    struct RelocationHeader
    {
        uint32_t cAbsolute;
        uint32_t cRelative;
    };

    struct Relocation
    {
        uint16_t skip;
        uint16_t patch;
    };
} // namespace _3DSX
