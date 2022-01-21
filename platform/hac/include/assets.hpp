#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace Assets
{
    static constexpr char MAGIC[4] = { 'A', 'S', 'E', 'T' };

    struct Section
    {
        uint64_t offset;
        uint64_t size;
    };

    static constexpr size_t SECTION_SIZE = sizeof(Section);
    static_assert(SECTION_SIZE == 0x10);

    struct Header
    {
        char magic[4];
        uint32_t version;
        Assets::Section icon;
        Assets::Section nacp;
        Assets::Section romfs;
    };

    static constexpr size_t HEADER_SIZE = sizeof(Header);
    static_assert(HEADER_SIZE == 0x38);
} // namespace Assets
