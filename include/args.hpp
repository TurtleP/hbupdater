#pragma once

#include <getopt.h>
#include <stdio.h>

#include <string>
#include <vector>

namespace args
{
    enum ParseStatus
    {
        PARSE_SUCCESS,
        PARSE_FAILURE,
        PARSE_EXIT,
    };

    struct Info
    {
        const char* filepath;  //< Filepath for dump
        const char* smdhPath;  //< Path for smdh file
        const char* romfsPath; //< Path for RomFS
        const char* outPath;
    };

    ParseStatus parse(args::Info& info, std::vector<char*>& args);

    inline int usage(const char* programName)
    {
        fprintf(stderr,
                "Usage: %s [OPTIONS...] <input>\n"
                "  Options:\n"
                "    help      Show this help message\n"
                "    -s --smdh <file> Filepath to the new SMDH file\n"
                "    -r --romfs <file> Filepath to the new RomFS\n"
                "    -o --output <file> Output file"
                "  Arguments:\n"
                "    input  Filepath to the 3dsx file"
                "    output Filepath to output as",
                programName);

        return 0;
    }

    // clang-format off
    const struct option long_options[] =
    {
        { "help",   no_argument,       nullptr, 'h' },
        { "input",  required_argument, nullptr, 0   },
        { "smdh",   optional_argument, nullptr, 's' },
        { "romfs",  optional_argument, nullptr, 'r' },
        { "output", required_argument, nullptr, 'o' },
        { nullptr,  no_argument,       nullptr, 0   }
    };
    // clang-format on
} // namespace args
