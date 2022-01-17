#pragma once

#include <stdint.h>

namespace SMDH
{
    static constexpr uint32_t MAGIC = 1212435795;

    /**
     * @brief SMDH Title Info
     * https://www.3dbrew.org/wiki/SMDH#Application_Titles
     */
    struct Title
    {
        uint16_t shortDescription[0x40];
        uint16_t longDescription[0x80];
        uint16_t publisher[0x40];
    };

    /**
     * @brief SMDH Application Settings
     * https://www.3dbrew.org/wiki/SMDH#Application_Settings
     */
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

    /**
     * @brief SMDH Header
     * https://www.3dbrew.org/wiki/SMDH#Format
     */
    struct Header
    {
        uint32_t magic;
        uint16_t version;
        uint16_t zero;
        SMDH::Title titles[0x10];
        SMDH::Settings settings;
        uint8_t zeroTwo[0x08];
    };

    static constexpr size_t HEADER_SIZE = sizeof(Header);
    static_assert(HEADER_SIZE == 0x2040);
}; // namespace SMDH
