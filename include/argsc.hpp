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

    struct IInfo
    {
        const char* filepath;  //< Filepath for binary
        const char* romfsPath; //< Path for RomFS
        const char* outPath;   //< Filepath for output
    };

    struct Info;

    ParseStatus parse(args::Info& info, std::vector<char*>& args);
} // namespace args
