#include "args.hpp"

#include <assert.h>
#include <cstring>

args::ParseStatus args::parse(args::Info& info, std::vector<char*>& arguments)
{
    int value = 0;

    if (arguments.size() > 1)
        info.filepath = arguments.at(1);
    else
    {
        fputs("No input file provided.\n", stderr);
        return args::PARSE_FAILURE;
    }

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
            case 's':
                info.smdhPath = optarg;
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

    if (!info.smdhPath)
        info.smdhPath = "";

    if (!info.romfsPath)
        info.romfsPath = "";

    assert(optind >= 0);

    if (!info.outPath)
    {
        fputs("No output name provided.\n", stderr);
        return args::PARSE_FAILURE;
    }

    return args::PARSE_SUCCESS;
}
