// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "core/serializable.hpp"

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

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(magic);
                archive.property(length);
                archive.property(hash);
                archive.property(fileType);
            }
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
      public:
        virtual auto loadFromFile(std::filesystem::path const& filePath) -> bool = 0;

        virtual auto loadFromBytes(std::span<uint8_t const> const dataBytes) -> bool = 0;
    };
} // namespace ionengine