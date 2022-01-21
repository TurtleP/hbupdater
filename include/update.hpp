#pragma once

#include "args.hpp"

#include <vector>

#define errorf(format, ...)              \
    {                                    \
        printf((format), ##__VA_ARGS__); \
        return -1;                       \
    };

#define error(str)             \
    {                          \
        errorf("%s\n", (str)); \
        return -1;             \
    }

namespace update
{
    int init(const args::Info& args);

    std::vector<uint8_t> read(const char* filepath);
} // namespace update
