#pragma once

#include "argsc.hpp"

namespace args
{
    struct Info : IInfo
    {
        const char* smdhPath; //< Path for smdh file
    };

    inline int usage(const char* programName)
    {
        fprintf(stderr,
                "Usage: %s [OPTIONS...] <input>\n"
                "  Options:\n"
                "    help      Show this help message\n"
                "    -s --smdh <file> Filepath to the new SMDH file\n"
                "    -r --romfs <file> Filepath to the new RomFS\n"
                "    -o --output <file> Output file\n"
                "  Arguments:\n"
                "    input  Filepath to the 3dsx file",
                programName);

        return 0;
    }

    // clang-format off
    const struct option long_options[] =
    {
        { "help",   no_argument,       nullptr, 'h' },
        { "smdh",   required_argument, nullptr, 's' },
        { "romfs",  required_argument, nullptr, 'r' },
        { "output", required_argument, nullptr, 'o' },
        { nullptr,  no_argument,       nullptr, 0   }
    };
    // clang-format on
} // namespace args
