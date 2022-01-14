#pragma once

#include "types.hpp"

namespace SMDH
{
    static constexpr uint32_t MAGIC = 1212435795;

    struct Title
    {
        uint16_t shortDescription[0x40];
        uint16_t longDescription[0x80];
        uint16_t publisher[0x40];
    };

    struct Settings
    {
        uint8_t ratings[0x10];
        uint32_t regionLockout;
        uint8_t matchmakerId[0xC];
        uint32_t flags;
        uint16_t eulaVersion;
        uint16_t zero;
        uint32_t optimalBannerFrame;
        uint32_t streetpassId;
    };

    struct Header
    {
        uint32_t magic;
        uint16_t version;
        uint16_t zero;
        SMDH::Title titles[0x10];
        SMDH::Settings settings;
        uint8_t zeroTwo[0x8];
    };
}; // namespace SMDH
