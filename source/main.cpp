#include "3dsx.hpp"
#include "smdh.hpp"

#include "args.hpp"

#include <stdio.h>
#include <string.h>

#define die(msg)                   \
    do                             \
    {                              \
        fputs(msg "\n\n", stderr); \
        return 1;                  \
    } while (0)

#define safe_call(a)   \
    do                 \
    {                  \
        int rc = a;    \
        if (rc != 0)   \
            return rc; \
    } while (0)

static bool g_dumpMode;

int parseArgs(args::Info& info, int argc, char* argv[])
{
    if (argc == 1)
        return args::usage(argv[0]);

    int status = 0;

    for (size_t index = 1; index < argc; index++)
    {
        char* arg = argv[index];
        switch (status++)
        {
            case 0:
            {
                if (strncmp(arg, "help", 4) != 0)
                {
                    info.mode  = arg;
                    g_dumpMode = (strncmp(arg, "dump", 4) == 0);
                }
                else
                    return args::usage(argv[0]);

                break;
            }
            case 1:
            {
                if (g_dumpMode)
                    info.binaryPath = arg;
                else
                    info.filepath = arg;

                break;
            }
            case 2:
            {
                if (g_dumpMode)
                    info.smdhPath = arg;
                else
                    info.output = arg;

                break;
            }
            case 3:
                info.romfsPath = arg;
                break;
            case 4:
                info.output = arg;
                break;
            default:
                return args::usage(argv[0]);
        }
    }

    if (g_dumpMode)
    {
        if (status < 3)
            return args::dump(argv[0]);
    }
    else
    {
        if (status < 5)
            return args::pack(argv[0]);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    args::Info info {};
    safe_call(parseArgs(info, argc, argv));
}
