

#pragma once

namespace ionengine {

enum class DDS : uint32_t {
    Magic = 0x20534444
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
    std::vector<char8_t> data2;
};

class DDSLoader {
public:

    DDSLoader() = default;

    bool parse(std::span<char8_t> const data);


private:


};

}