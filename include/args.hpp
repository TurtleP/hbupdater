#include <stdio.h>

namespace args
{
    struct Info
    {
        char* mode;   //< Application mode
        char* output; //< Output dir/filename

        char* filepath; //< Filepath for dump

        char* binaryPath; //< Path for binary data
        char* smdhPath;   //< Path for smdh file
        char* romfsPath;  //< Path for RomFS
    };

    inline int usage(const char* programName)
    {
        fprintf(stderr,
                "Usage: %s [OPTIONS...] <mode> <args>\n"
                "  Options:\n"
                "    help  Show this help message\n"
                "  Modes:\n"
                "    dump  Dump a 3dsx to its binary, SMDH, and RomFS components\n"
                "    pack  Pack a 3dsx from binary, SMDH, and RomFS components",
                programName);

        return 0;
    }

    inline int dump(const char* programName)
    {
        fprintf(stderr, "Usage: %s dump <filepath> <directory>", programName);

        return 0;
    }

    inline int pack(const char* programName)
    {
        fprintf(stderr, "Usage: %s pack <binary> <smdh> <romfs> <output>.3dsx", programName);

        return 0;
    }
} // namespace args
