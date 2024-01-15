
#include "precompiled.h"
#include "ktx2_image.hpp"

using namespace ionengine;

Ktx2Image::Ktx2Image(std::span<uint8_t> const data_bytes) {

	std::basic_ispanstream<uint8_t> stream(data_bytes, std::ios::binary);

	std::array<uint8_t, sizeof(KTX_MAGIC)> identifier;
	stream.read(identifier.data(), 12);

    if(std::memcmp(identifier.data(), KTX_MAGIC.data(), sizeof(KTX_MAGIC)) != 0) {
        throw core::Exception("Trying to parse a corrupted ktx2 image");
    }

	uint32_t format;
	stream.read((uint8_t *)&format, sizeof(uint32_t));
	uint32_t type_size;
	stream.read((uint8_t *)&type_size, sizeof(uint32_t));
	uint32_t pixel_width;
	stream.read((uint8_t *)&pixel_width, sizeof(uint32_t));
	uint32_t pixel_height;
	stream.read((uint8_t *)&pixel_height, sizeof(uint32_t));
	uint32_t pixel_depth;
	stream.read((uint8_t *)&pixel_depth, sizeof(uint32_t));
	uint32_t layer_count;
	stream.read((uint8_t *)&layer_count, sizeof(uint32_t));
	uint32_t face_count;
	stream.read((uint8_t *)&face_count, sizeof(uint32_t));
	uint32_t level_count;
	stream.read((uint8_t *)&level_count, sizeof(uint32_t));
	uint32_t supercompression_scheme;
	stream.read((uint8_t *)&supercompression_scheme, sizeof(uint32_t));

	switch (supercompression_scheme) {
        case 1: {
            throw core::Exception("BasisLZ is not supported by ktx2 image loader");
        } break;
        case 2: {
            throw core::Exception("Zstandard is not supported by ktx2 image loader");
        } break;
        case 3: {
            throw core::Exception("ZLIB is not supported by ktx2 image loader");
        } break;
	}

	stream.seekg(4 * sizeof(uint32_t) + 2 * sizeof(uint64_t), std::ios::cur);

	std::vector<MipLevelData> mip_level_data(level_count);
	for (uint32_t i = 0; i < level_count; ++i) {
		stream.read((uint8_t *)&mip_level_data[i], sizeof(MipLevelData));
	}

	std::vector<std::vector<uint8_t>> image_data(level_count);
	for (uint32_t i = 0; i < level_count; ++i) {
		stream.seekg(mip_level_data[i].byte_offset, std::ios::beg);

		switch (supercompression_scheme) {
            case 0: {
                image_data[i].resize(mip_level_data[i].byte_length);
                stream.read((uint8_t *)image_data[i].data(), mip_level_data[i].byte_length);
            } break;
		}
	}
}