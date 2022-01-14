#pragma once

#include <stdio.h>

namespace args
{
    struct Info
    {
        char* filepath;  //< Filepath for dump
        char* smdhPath;  //< Path for smdh file
        char* romfsPath; //< Path for RomFS
        char* outPath;
    };

    inline int usage(const char* programName)
    {
        fprintf(stderr,
                "Usage: %s [OPTIONS...] <smdh> <romfs> <output>\n"
                "  Options:\n"
                "    help  Show this help message\n"
                "  Arguments:\n"
                "    smdh   Filepath to the new SMDH file\n"
                "    romfs  Filepath to the new RomFS\n"
                "    output Filepath to output as",
                programName);

        return 0;
    }
} // namespace args
