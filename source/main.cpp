#include "args.hpp"
#include "update.hpp"

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
                    info.filepath = arg;
                else
                    return args::usage(argv[0]);

                break;
            }
            case 1:
                info.smdhPath = arg;
                break;
            case 2:
                info.romfsPath = arg;
                break;
            case 3:
                info.outPath = arg;
                break;
            default:
                return args::usage(argv[0]);
        }
    }

    if (status < 4)
        return args::usage(argv[0]);

    return 0;
}

int main(int argc, char* argv[])
{
    args::Info info {};
    safe_call(parseArgs(info, argc, argv));

    safe_call(update::init(info));
}
