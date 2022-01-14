#pragma once

#include "3dsx.hpp"
#include "smdh.hpp"

#include "args.hpp"

namespace update
{
    int init(const args::Info& info);

    uint8_t* read(char* smdhPath, uint32_t& size);
} // namespace update
