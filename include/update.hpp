#pragma once

#include "args.hpp"

#include <vector>

namespace update
{
    int init(const args::Info& args);

    std::vector<uint8_t> read(const char* filepath);
} // namespace update
