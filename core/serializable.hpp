// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename Type>
    class Serializable
    {
      public:
        static auto loadFromMemory(std::span<uint8_t const> const data) -> std::optional<Type>;

        static auto loadFromFile(std::filesystem::path const& filePath) -> std::optional<Type>;

        static auto saveToFile(Type const& object, std::filesystem::path const& filePath) -> bool;

        static auto saveToMemory(Type const& object, std::basic_stringstream<uint8_t>& stream) -> bool;
    };
} // namespace ionengine::core