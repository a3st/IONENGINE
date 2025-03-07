// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "crc32.hpp"
#include "precompiled.h"

namespace ionengine::core
{
    auto CRC32::encode(std::string_view const source) -> uint32_t
    {
        uint32_t crc = 0xffffffff;
        for (auto c : source)
        {
            crc = (crc >> 8) ^ CRC32::crc_table[(crc ^ c) & 0xff];
        }
        return crc ^ 0xffffffff;
    }
} // namespace ionengine::core