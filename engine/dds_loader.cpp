// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/dds_loader.h>

using namespace ionengine;

bool DDSLoader::parse(std::span<char8_t> const data) {

    auto read_bytes = [&](std::span<char8_t> const src, uint64_t& offset, std::span<char8_t>& dst, uint64_t const size) -> size_t {
        size_t read_bytes = 0;
        if(offset + size <= src.size()) {
            read_bytes = size;
            dst = std::span<char8_t>(src.data() + offset, src.data() + size);
        }
        offset += read_bytes;
        return read_bytes;
    };

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&_file_layout.magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    if(_file_layout.magic != static_cast<uint32_t>(DDS::Magic)) {
        return false;
    }

    if(read_bytes(data, offset, buffer, sizeof(DDSHeader)) > 0) {
        std::memcpy(&_file_layout.header, buffer.data(), buffer.size());
    } else {
        return false;
    }

    size_t mips_size = data.size() - (sizeof(DDSHeader) + sizeof(uint32_t));
    _file_layout.data.resize(mips_size);

    if(read_bytes(data, offset, buffer, mips_size) > 0) {
        std::memcpy(_file_layout.data.data(), buffer.data(), buffer.size());
    } else {
        return false;
    }

    return true;
}
