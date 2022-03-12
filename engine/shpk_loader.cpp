// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/shpk_loader.h>

using namespace ionengine;

bool ShpkLoader::parse(std::span<char8_t> const data) {

    auto read_bytes = [&](std::span<char8_t> const src, uint64_t& offset, std::span<char8_t>& dst, uint64_t const size) -> size_t {
        size_t read_bytes = 0;
        if(offset + size <= src.size()) {
            read_bytes = size;
            dst = std::span<char8_t>(src.data() + offset, size);
        }
        offset += read_bytes;
        return read_bytes;
    };

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&_hlsl_spirv_file.magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    if(_hlsl_spirv_file.magic != static_cast<uint32_t>(Shpk::Magic)) {
        return false;
    }

    if(read_bytes(data, offset, buffer, sizeof(HLSL_SPIRV_Header)) > 0) {
        std::memcpy(&_hlsl_spirv_file.header, buffer.data(), buffer.size());
    } else {
        return false;
    }

    size_t info_size = _hlsl_spirv_file.header.shaders_count * sizeof(HLSL_SPIRV_Info);
    _hlsl_spirv_file.info.resize(info_size);

    if(read_bytes(data, offset, buffer, info_size) > 0) {
        std::memcpy(_hlsl_spirv_file.info.data(), buffer.data(), buffer.size());
    } else {
        return false;
    }

    size_t data_size = data.size() - (sizeof(uint32_t) + sizeof(HLSL_SPIRV_Header) + info_size);
    _hlsl_spirv_file.data.resize(data_size);

    if(read_bytes(data, offset, buffer, data_size) > 0) {
        std::memcpy(_hlsl_spirv_file.data.data(), buffer.data(), buffer.size());
    } else {
        return false;
    }

    return true;
}
