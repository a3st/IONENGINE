
#pragma once

#include <spanstream>

namespace ionengine {

class Ktx2Image {
public:

    Ktx2Image(std::span<uint8_t> const data_bytes) {

        std::basic_ispanstream<uint8_t> stream(data_bytes, std::ios::binary);

        std::array<uint8_t, 12> identifier;
        stream.read(identifier.data(), 12);
        uint32_t format;
        stream.read((uint8_t*)&format, sizeof(uint32_t));
        uint32_t type_size;
        stream.read((uint8_t*)&type_size, sizeof(uint32_t));
        uint32_t pixel_width;
        stream.read((uint8_t*)&pixel_width, sizeof(uint32_t));
        uint32_t pixel_height;
        stream.read((uint8_t*)&pixel_height, sizeof(uint32_t));
        uint32_t pixel_depth;
        stream.read((uint8_t*)&pixel_depth, sizeof(uint32_t));
        uint32_t layer_count;
        stream.read((uint8_t*)&layer_count, sizeof(uint32_t));
        uint32_t face_count;
        stream.read((uint8_t*)&face_count, sizeof(uint32_t));
        uint32_t level_count;
        stream.read((uint8_t*)&level_count, sizeof(uint32_t));
        uint32_t supercompression_scheme;
        stream.read((uint8_t*)&supercompression_scheme, sizeof(uint32_t));

        std::cout << std::string_view((char*)identifier.data(), identifier.size()) << std::endl;
        std::cout << std::format("{} {}", pixel_width, pixel_height) << std::endl;
    }

    

private:

};

}