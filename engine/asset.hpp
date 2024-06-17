// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine
{
    namespace asset
    {
        std::array<uint8_t, 8> constexpr Magic{'A', 'S', 'S', 'E', 'T', 'I', 'O', 'F'};

        struct Header
        {
            std::array<uint8_t, Magic.size()> magic;
            uint64_t length;
            std::array<uint8_t, 16> hash;
            std::array<uint8_t, 8> fileType;
        };

        enum class ChunkType : uint32_t
        {
            JSON,
            BIN
        };

        struct ChunkHeader
        {
            uint32_t chunkType;
            uint32_t chunkLength;
        };
    } // namespace asset

    inline auto getAssetHeader(std::basic_istream<uint8_t>& stream) -> std::optional<asset::Header>
    {
        asset::Header header;
        stream.read((uint8_t*)&header, sizeof(asset::Header));

        if (std::memcmp(header.magic.data(), asset::Magic.data(), asset::Magic.size()) != 0)
        {
            return std::nullopt;
        }
        return header;
    }

    class Asset : public core::ref_counted_object
    {
    };
} // namespace ionengine