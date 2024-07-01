// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    // https://github.com/matheusgomes28/base64pp
    namespace internal
    {
        inline std::array<char, 64> constexpr encodeTable{
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

        inline std::array<uint8_t, 256> constexpr decodeTable{
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

        inline std::array<char, 4> encodeTripplet(uint8_t const a, uint8_t const b, uint8_t const c)
        {
            uint32_t const concatBits = (a << 16) | (b << 8) | c;

            auto const b64Char1 = encodeTable[(concatBits >> 18) & 0b0011'1111];
            auto const b64Char2 = encodeTable[(concatBits >> 12) & 0b0011'1111];
            auto const b64Char3 = encodeTable[(concatBits >> 6) & 0b0011'1111];
            auto const b64Char4 = encodeTable[concatBits & 0b0011'1111];

            return {b64Char1, b64Char2, b64Char3, b64Char4};
        }

        inline bool isValidBase64Char(char c)
        {
            auto const decode_byte = decodeTable[c];
            return decode_byte != 0x64;
        }

        inline bool isValidBase64Str(std::string_view const encodedStr)
        {
            if ((encodedStr.size() % 4) == 1)
            {
                return false;
            }

            if (!std::all_of(std::begin(encodedStr), std::end(encodedStr) - 2,
                             [](char c) { return isValidBase64Char(c); }))
            {
                return false;
            }

            auto const last = std::rbegin(encodedStr);
            if (!isValidBase64Char(*std::next(last)))
            {
                return (*std::next(last) == '=') && (*last == '=');
            }

            return isValidBase64Char(*last) || (*last == '=');
        }

        inline std::array<uint8_t, 3> decodeQuad(char const a, char const b, char const c, char const d)
        {
            uint32_t const concatBytes =
                (decodeTable[a] << 18) | (decodeTable[b] << 12) | (decodeTable[c] << 6) | decodeTable[d];

            uint8_t const byte1 = (concatBytes >> 16) & 0b1111'1111;
            uint8_t const byte2 = (concatBytes >> 8) & 0b1111'1111;
            uint8_t const byte3 = concatBytes & 0b1111'1111;
            return {byte1, byte2, byte3};
        }
    } // namespace internal

    namespace base64
    {
        inline std::string encode(std::span<uint8_t const> const source)
        {
            auto const size = source.size();
            auto const fullTripples = size / 3;

            std::string output;
            output.reserve((fullTripples + 2) * 4);

            for (std::size_t i = 0; i < fullTripples; ++i)
            {
                auto const tripplet = source.subspan(i * 3, 3);
                auto const base64Chars = internal::encodeTripplet(tripplet[0], tripplet[1], tripplet[2]);
                std::copy(std::begin(base64Chars), std::end(base64Chars), std::back_inserter(output));
            }

            if (auto const remainingChars = size - fullTripples * 3; remainingChars == 2)
            {
                auto const lastTwo = source.last(2);
                auto const base64Chars = internal::encodeTripplet(lastTwo[0], lastTwo[1], 0x00);

                output.push_back(base64Chars[0]);
                output.push_back(base64Chars[1]);
                output.push_back(base64Chars[2]);
                output.push_back('=');
            }
            else if (remainingChars == 1)
            {
                auto const base64Chars = internal::encodeTripplet(source.back(), 0x00, 0x00);

                output.push_back(base64Chars[0]);
                output.push_back(base64Chars[1]);
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

            if (!internal::isValidBase64Str(source))
            {
                return std::nullopt;
            }

            auto const unpaddedSource = source.substr(0, source.find_first_of('='));
            auto const fullQuadruples = unpaddedSource.size() / 4;

            std::vector<std::uint8_t> decodedBytes;
            decodedBytes.reserve(((fullQuadruples + 2) * 3) / 4);

            for (std::size_t i = 0; i < fullQuadruples; ++i)
            {
                auto const quad = unpaddedSource.substr(i * 4, 4);
                auto const bytes = internal::decodeQuad(quad[0], quad[1], quad[2], quad[3]);
                std::copy(std::begin(bytes), std::end(bytes), std::back_inserter(decodedBytes));
            }

            if (auto const lastQuad = unpaddedSource.substr(fullQuadruples * 4); lastQuad.size() == 0)
            {
                return decodedBytes;
            }
            else if ((lastQuad.size() == 2) || (lastQuad[2] == '='))
            {
                auto const bytes = internal::decodeQuad(lastQuad[0], lastQuad[1], 'A', 'A');
                decodedBytes.push_back(bytes[0]);
            }
            else
            {
                auto const bytes = internal::decodeQuad(lastQuad[0], lastQuad[1], lastQuad[2], 'A');
                std::copy_n(std::begin(bytes), 2, std::back_inserter(decodedBytes));
            }
            return decodedBytes;
        }
    } // namespace base64
} // namespace ionengine::core