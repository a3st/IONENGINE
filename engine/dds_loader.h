// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

enum class DDSFlags : uint32_t {
    RGB = 0x00000040,
    Luminance = 0x00020000,
    Alpha = 0x00000002,
    FourCC = 0x00000004
};

DECLARE_ENUM_CLASS_BIT_FLAG(DDSFlags)

enum class DDSFourCC {
    DXT1 = ((uint32_t)(uint8_t)'D') | ((uint32_t)(uint8_t)'X' << 8) | ((uint32_t)(uint8_t)'T' << 16) | ((uint32_t)(uint8_t)'1' << 24),
    DXT2 = ((uint32_t)(uint8_t)'D') | ((uint32_t)(uint8_t)'X' << 8) | ((uint32_t)(uint8_t)'T' << 16) | ((uint32_t)(uint8_t)'2' << 24),
    DXT3 = ((uint32_t)(uint8_t)'D') | ((uint32_t)(uint8_t)'X' << 8) | ((uint32_t)(uint8_t)'T' << 16) | ((uint32_t)(uint8_t)'3' << 24),
    DXT4 = ((uint32_t)(uint8_t)'D') | ((uint32_t)(uint8_t)'X' << 8) | ((uint32_t)(uint8_t)'T' << 16) | ((uint32_t)(uint8_t)'4' << 24),
    DXT5 = ((uint32_t)(uint8_t)'D') | ((uint32_t)(uint8_t)'X' << 8) | ((uint32_t)(uint8_t)'T' << 16) | ((uint32_t)(uint8_t)'5' << 24),
    BC4U = ((uint32_t)(uint8_t)'B') | ((uint32_t)(uint8_t)'C' << 8) | ((uint32_t)(uint8_t)'4' << 16) | ((uint32_t)(uint8_t)'U' << 24),
    BC4S = ((uint32_t)(uint8_t)'B') | ((uint32_t)(uint8_t)'C' << 8) | ((uint32_t)(uint8_t)'4' << 16) | ((uint32_t)(uint8_t)'S' << 24),
    BC5U = ((uint32_t)(uint8_t)'B') | ((uint32_t)(uint8_t)'C' << 8) | ((uint32_t)(uint8_t)'5' << 16) | ((uint32_t)(uint8_t)'U' << 24),
    BC5S = ((uint32_t)(uint8_t)'B') | ((uint32_t)(uint8_t)'C' << 8) | ((uint32_t)(uint8_t)'5' << 16) | ((uint32_t)(uint8_t)'S' << 24)
};

struct DDSPixelFormat {
    uint32_t size;
    uint32_t flags;
    uint32_t four_cc;
    uint32_t rgb_bit_count;
    uint32_t r_bit_mask;
    uint32_t g_bit_mask;
    uint32_t b_bit_mask;
    uint32_t a_bit_mask;
};

struct DDSHeader {
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t linear_size;
    uint32_t depth;
    uint32_t mip_count;
    std::array<uint32_t, 11> reserved1;
    DDSPixelFormat pf;
    uint32_t caps;
    uint32_t caps2;
    uint32_t caps3;
    uint32_t caps4;
    uint32_t reserved2;
};

struct DDSFileLayout {
    uint32_t magic;
    DDSHeader header;
    std::vector<char8_t> data;
};

class DDSLoader {
public:

    DDSLoader() = default;

    bool parse(std::span<char8_t> const data);

    const DDSFileLayout* data() const { return &_file_layout; }

private:

    enum class DDS : uint32_t {
        Magic = 0x20534444
    };

    DDSFileLayout _file_layout;
};

}
