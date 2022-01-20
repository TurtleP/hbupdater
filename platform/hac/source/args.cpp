#include "args.hpp"

#include <assert.h>
#include <cstring>

args::ParseStatus args::parse(args::Info& info, std::vector<char*>& arguments)
{
    int value = 0;

    // parse options
    while (true)
    {
        value = getopt_long(arguments.size(), arguments.data(), "h:s:r:o:", args::long_options,
                            nullptr);

        if (value == -1)
            break;

        switch (value)
        {
            case 'h':
            {
                args::usage(arguments.at(0));
                return args::PARSE_EXIT;
            }
            case 'n':
                info.nacpPath = optarg;
                break;
            case 'i':
                info.iconPath = optarg;
                break;
            case 'r':
                info.romfsPath = optarg;
                break;
            case 'o':
                info.outPath = optarg;
                break;
            default:
            {
                fprintf(stderr, "Invalid option '%c'\n", optopt);
                return args::PARSE_FAILURE;
            }
        }
    }

    if (!info.iconPath)
        info.iconPath = "";

    if (!info.nacpPath)
        info.nacpPath = "";

    if (!info.romfsPath)
        info.romfsPath = "";

    assert(optind >= 0);

    if (static_cast<size_t>(optind) < arguments.size())
        info.filepath = arguments[optind++];

    if (!info.filepath)
    {
        fputs("No input file provided.\n", stderr);
        return args::PARSE_FAILURE;
    }

    if (!info.outPath)
    {
        fputs("No output name provided.\n", stderr);
        return args::PARSE_FAILURE;
    }

    return args::PARSE_SUCCESS;
}
