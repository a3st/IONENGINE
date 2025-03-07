// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "base64.hpp"
#include "precompiled.h"

namespace ionengine::core
{
    auto Base64::encode_tripplet(uint8_t const a, uint8_t const b, uint8_t const c) -> std::array<char, 4>
    {
        uint32_t const concat_bits = (a << 16) | (b << 8) | c;

        auto const b64_char1 = encode_table[(concat_bits >> 18) & 0b0011'1111];
        auto const b64_char2 = encode_table[(concat_bits >> 12) & 0b0011'1111];
        auto const b64_char3 = encode_table[(concat_bits >> 6) & 0b0011'1111];
        auto const b64_char4 = encode_table[concat_bits & 0b0011'1111];

        return {b64_char1, b64_char2, b64_char3, b64_char4};
    }

    auto Base64::is_valid_base64_char(char c) -> bool
    {
        auto const decode_byte = decode_table[c];
        return decode_byte != 0x64;
    }

    auto Base64::is_valid_base64_str(std::string_view const encoded_str) -> bool
    {
        if ((encoded_str.size() % 4) == 1)
        {
            return false;
        }

        if (!std::all_of(std::begin(encoded_str), std::end(encoded_str) - 2,
                         [](char c) { return is_valid_base64_char(c); }))
        {
            return false;
        }

        auto const last = std::rbegin(encoded_str);
        if (!is_valid_base64_char(*std::next(last)))
        {
            return (*std::next(last) == '=') && (*last == '=');
        }

        return is_valid_base64_char(*last) || (*last == '=');
    }

    auto Base64::decode_quad(char const a, char const b, char const c, char const d) -> std::array<uint8_t, 3>
    {
        uint32_t const concat_bytes =
            (decode_table[a] << 18) | (decode_table[b] << 12) | (decode_table[c] << 6) | decode_table[d];

        uint8_t const byte1 = (concat_bytes >> 16) & 0b1111'1111;
        uint8_t const byte2 = (concat_bytes >> 8) & 0b1111'1111;
        uint8_t const byte3 = concat_bytes & 0b1111'1111;
        return {byte1, byte2, byte3};
    }

    auto Base64::encode(std::span<uint8_t const> const source) -> std::string
    {
        auto const size = source.size();
        auto const full_tripples = size / 3;

        std::string output;
        output.reserve((full_tripples + 2) * 4);

        for (size_t const i : std::views::iota(0u, full_tripples))
        {
            auto const tripplet = source.subspan(i * 3, 3);
            auto const base64_chars = Base64::encode_tripplet(tripplet[0], tripplet[1], tripplet[2]);
            std::copy(std::begin(base64_chars), std::end(base64_chars), std::back_inserter(output));
        }

        if (auto const remaining_chars = size - full_tripples * 3; remaining_chars == 2)
        {
            auto const last_two = source.last(2);
            auto const base64_chars = Base64::encode_tripplet(last_two[0], last_two[1], 0x00);

            output.push_back(base64_chars[0]);
            output.push_back(base64_chars[1]);
            output.push_back(base64_chars[2]);
            output.push_back('=');
        }
        else if (remaining_chars == 1)
        {
            auto const base64_chars = Base64::encode_tripplet(source.back(), 0x00, 0x00);

            output.push_back(base64_chars[0]);
            output.push_back(base64_chars[1]);
            output.push_back('=');
            output.push_back('=');
        }
        return output;
    }

    auto Base64::decode(std::string_view const source) -> std::optional<std::vector<std::uint8_t>>
    {
        if (source.size() == 0)
        {
            return std::nullopt;
        }

        if (!Base64::is_valid_base64_str(source))
        {
            return std::nullopt;
        }

        auto const unpadded_source = source.substr(0, source.find_first_of('='));
        auto const full_quadruples = unpadded_source.size() / 4;

        std::vector<std::uint8_t> decoded_bytes;
        decoded_bytes.reserve(((full_quadruples + 2) * 3) / 4);

        for (size_t const i : std::views::iota(0u, full_quadruples))
        {
            auto const quad = unpadded_source.substr(i * 4, 4);
            auto const bytes = Base64::decode_quad(quad[0], quad[1], quad[2], quad[3]);
            std::copy(std::begin(bytes), std::end(bytes), std::back_inserter(decoded_bytes));
        }

        if (auto const last_quad = unpadded_source.substr(full_quadruples * 4); last_quad.size() == 0)
        {
            return decoded_bytes;
        }
        else if ((last_quad.size() == 2) || (last_quad[2] == '='))
        {
            auto const bytes = Base64::decode_quad(last_quad[0], last_quad[1], 'A', 'A');
            decoded_bytes.push_back(bytes[0]);
        }
        else
        {
            auto const bytes = Base64::decode_quad(last_quad[0], last_quad[1], last_quad[2], 'A');
            std::copy_n(std::begin(bytes), 2, std::back_inserter(decoded_bytes));
        }
        return decoded_bytes;
    }
} // namespace ionengine::core