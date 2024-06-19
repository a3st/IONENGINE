// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename Type>
    class Serializable
    {
      public:
        static auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<Type>;

        static auto serialize(Type const& object, std::basic_ostream<uint8_t>& stream) -> size_t;
    };

    template <typename Type>
    auto loadFromBytes(std::span<uint8_t const> const dataBytes) -> std::optional<Type>
    {
        std::basic_ispanstream<uint8_t> stream(
            std::span<uint8_t>(const_cast<uint8_t*>(dataBytes.data()), dataBytes.size()), std::ios::binary);
        return Serializable<Type>::deserialize(stream);
    }

    template <typename Type>
    auto loadFromFile(std::filesystem::path const& filePath) -> std::optional<Type>
    {
        std::basic_ifstream<uint8_t> stream(filePath, std::ios::binary);
        if (!stream.is_open())
        {
            return std::nullopt;
        }
        return Serializable<Type>::deserialize(stream);
    }

    template <typename Type>
    auto saveToFile(Type const& object, std::filesystem::path const& filePath) -> bool
    {
        std::basic_ofstream<uint8_t> stream(filePath, std::ios::binary);
        if (!stream.is_open())
        {
            return false;
        }
        return Serializable<Type>::serialize(object, stream) > 0;
    }
} // namespace ionengine::core