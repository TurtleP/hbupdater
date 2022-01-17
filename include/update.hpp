#pragma once

#include "3dsx.hpp"
#include "romfs.hpp"
#include "smdh.hpp"

#include "args.hpp"

#include <vector>

namespace update
{
    int init(const args::Info& args);

    std::vector<uint8_t> read(char* filepath);
} // namespace update
