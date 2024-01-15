
#include "precompiled.h"
#include "glb_model.hpp"
#include <simdjson.h>

using namespace ionengine;

GLBModel::GLBModel(std::span<uint8_t> const data_bytes) {

    std::basic_ispanstream<uint8_t> stream(data_bytes, std::ios::binary);

    Header header;
	stream.read((uint8_t*)&header, sizeof(Header));

    ChunkHeader chunk_header;
    stream.read((uint8_t*)&chunk_header, sizeof(ChunkHeader));

    std::vector<uint8_t> chunk_data(chunk_header.chunk_length);
    stream.read((uint8_t*)chunk_data.data(), chunk_header.chunk_length);

    simdjson::ondemand::parser parser;
    auto document = parser.iterate(chunk_data.data(), chunk_data.size(), chunk_data.size() + simdjson::SIMDJSON_PADDING);

    //for(auto const& element : document["asset"]) {
    std::cout << document["asset"]["generator"].get_string() << std::endl;
    //}
}

