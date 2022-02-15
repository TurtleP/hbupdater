#include "argsc.hpp"
#include "update.hpp"
#include <cstring>
#include <stdio.h>
#include <string>

#include "args.hpp"

#define safe_call(a)   \
    do                 \
    {                  \
        int rc = a;    \
        if (rc != 0)   \
            return rc; \
    } while (0)

int main(int argc, char* argv[])
{
    args::Info info {};

    std::vector<char*> args(argv, argv + argc);
    switch (args::parse(info, args))
    {
        case args::PARSE_SUCCESS:
            break;
        case args::PARSE_FAILURE:
            return -1;
        case args::PARSE_EXIT:
            return -2;
    }

    safe_call(update::init(info));
}
