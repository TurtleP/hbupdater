#include <stdint.h>
#include <stdlib.h>

#pragma once

namespace NRO
{
    static constexpr char MAGIC[4] = { 'N', 'R', 'O', '0' };

    struct Start
    {
        uint32_t unused;
        uint32_t modZeroOffset;
        uint64_t padding;
    };

    static constexpr size_t START_SIZE = sizeof(Start);
    static_assert(START_SIZE == 0x10);

    struct SegmentHeader
    {
        uint32_t offset;
        uint32_t size;
    };

    static constexpr size_t SEGMENT_HEADER_SIZE = sizeof(SegmentHeader);
    static_assert(SEGMENT_HEADER_SIZE == 0x08);

    struct Header
    {
        char magic[4];
        uint32_t version;
        uint32_t totalSize;
        uint32_t flags;
        NRO::SegmentHeader segmentHeaders[3];
        uint32_t bssSize;
        uint32_t reserved;
        uint8_t moduleId[0x20];
        uint32_t dsoOffset;
        uint32_t reservedTwo;
        NRO::SegmentHeader segmentHeadersTwo[3];
    };

    static constexpr size_t HEADER_SIZE = sizeof(Header);
    static_assert(HEADER_SIZE == 0x70);
} // namespace NRO
