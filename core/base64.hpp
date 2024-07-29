// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    // https://github.com/matheusgomes28/base64pp
    namespace internal
    {
        inline std::array<char, 64> constexpr encode_table{
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

        inline std::array<uint8_t, 256> constexpr decode_table{
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x3E, 0x64, 0x64, 0x64, 0x3F, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
            0x3A, 0x3B, 0x3C, 0x3D, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
            0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
            0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
            0x64, 0x64, 0x64, 0x64};

        inline std::array<char, 4> encode_tripplet(uint8_t const a, uint8_t const b, uint8_t const c)
        {
            uint32_t const concat_bits = (a << 16) | (b << 8) | c;

            auto const b64_char1 = encode_table[(concat_bits >> 18) & 0b0011'1111];
            auto const b64_char2 = encode_table[(concat_bits >> 12) & 0b0011'1111];
            auto const b64_char3 = encode_table[(concat_bits >> 6) & 0b0011'1111];
            auto const b64_char4 = encode_table[concat_bits & 0b0011'1111];

            return {b64_char1, b64_char2, b64_char3, b64_char4};
        }

        inline bool is_valid_base64_char(char c)
        {
            auto const decode_byte = decode_table[c];
            return decode_byte != 0x64;
        }

        inline bool is_valid_base64_str(std::string_view const encoded_str)
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

        inline std::array<uint8_t, 3> decode_quad(char const a, char const b, char const c, char const d)
        {
            uint32_t const concat_bytes =
                (decode_table[a] << 18) | (decode_table[b] << 12) | (decode_table[c] << 6) | decode_table[d];

            uint8_t const byte1 = (concat_bytes >> 16) & 0b1111'1111;
            uint8_t const byte2 = (concat_bytes >> 8) & 0b1111'1111;
            uint8_t const byte3 = concat_bytes & 0b1111'1111;
            return {byte1, byte2, byte3};
        }
    } // namespace internal

    namespace base64
    {
        inline std::string encode(std::span<uint8_t const> const source)
        {
            auto const size = source.size();
            auto const full_tripples = size / 3;

            std::string output;
            output.reserve((full_tripples + 2) * 4);

            for (size_t const i : std::views::iota(0u, full_tripples))
            {
                auto const tripplet = source.subspan(i * 3, 3);
                auto const base64_chars = internal::encode_tripplet(tripplet[0], tripplet[1], tripplet[2]);
                std::copy(std::begin(base64_chars), std::end(base64_chars), std::back_inserter(output));
            }

            if (auto const remaining_chars = size - full_tripples * 3; remaining_chars == 2)
            {
                auto const last_two = source.last(2);
                auto const base64_chars = internal::encode_tripplet(last_two[0], last_two[1], 0x00);

                output.push_back(base64_chars[0]);
                output.push_back(base64_chars[1]);
                output.push_back(base64_chars[2]);
                output.push_back('=');
            }
            else if (remaining_chars == 1)
            {
                auto const base64_chars = internal::encode_tripplet(source.back(), 0x00, 0x00);

                output.push_back(base64_chars[0]);
                output.push_back(base64_chars[1]);
                output.push_back('=');
                output.push_back('=');
            }
            return output;
        }

        inline std::optional<std::vector<std::uint8_t>> decode(std::string_view source)
        {
            if (source.size() == 0)
            {
                return std::nullopt;
            }

            if (!internal::is_valid_base64_str(source))
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
                auto const bytes = internal::decode_quad(quad[0], quad[1], quad[2], quad[3]);
                std::copy(std::begin(bytes), std::end(bytes), std::back_inserter(decoded_bytes));
            }

            if (auto const last_quad = unpadded_source.substr(full_quadruples * 4); last_quad.size() == 0)
            {
                return decoded_bytes;
            }
            else if ((last_quad.size() == 2) || (last_quad[2] == '='))
            {
                auto const bytes = internal::decode_quad(last_quad[0], last_quad[1], 'A', 'A');
                decoded_bytes.push_back(bytes[0]);
            }
            else
            {
                auto const bytes = internal::decode_quad(last_quad[0], last_quad[1], last_quad[2], 'A');
                std::copy_n(std::begin(bytes), 2, std::back_inserter(decoded_bytes));
            }
            return decoded_bytes;
        }
    } // namespace base64
} // namespace ionengine::core