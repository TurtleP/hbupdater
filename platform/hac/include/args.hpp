#include "argsc.hpp"

namespace args
{
    struct Info : IInfo
    {
        const char* nacpPath; //< Path for nacp file
        const char* iconPath; //< Path for icon file
    };

    inline int usage(const char* programName)
    {
        fprintf(stderr,
                "Usage: %s [OPTIONS...] <input>\n"
                "  Options:\n"
                "    help      Show this help message\n"
                "    -n --nacp <file> Filepath to the new NACP file\n"
                "    -i --icon <file> Filepath to the new icon\n"
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
        { "nacp",   required_argument, nullptr, 'n' },
        { "icon",   required_argument, nullptr, 'i' },
        { "romfs",  required_argument, nullptr, 'r' },
        { "output", required_argument, nullptr, 'o' },
        { nullptr,  no_argument,       nullptr, 0   }
    };
    // clang-format on
} // namespace args
