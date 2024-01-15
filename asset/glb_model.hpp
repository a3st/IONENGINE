
#pragma once

#include "core/exception.hpp"

namespace ionengine {

class GLBModel {
public:

    GLBModel(std::span<uint8_t> const data_bytes);

private:

    inline static uint32_t const GLB_MAGIC = 0x46546C67;

    struct Header {
        uint32_t magic;
        uint32_t version;
        uint32_t length;
    };

    struct ChunkHeader {
        uint32_t chunk_length;
        uint32_t chunk_type;
    };

};

}