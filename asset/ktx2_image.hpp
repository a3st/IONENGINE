
#pragma once

#include "core/exception.hpp"

namespace ionengine {

class Ktx2Image {
public:

    Ktx2Image(std::span<uint8_t> const data_bytes);

private:

    inline static std::array<uint8_t, 12> const KTX_MAGIC{ 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

    struct MipLevelData {
        uint64_t byte_offset;
        uint64_t byte_length;
        uint64_t uncompressed_byte_length;
    };

};

}