// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <lib/algorithm.h>

using namespace ionengine;
using namespace ionengine::lib;

std::span<uint8_t const> ionengine::lib::read_bytes(std::span<uint8_t const> const source, uint64_t& offset, size_t const size) {
    
    size_t read_bytes = 0;
    std::span<uint8_t const> dest;

    if(offset + size <= source.size()) {
        read_bytes = size;
        dest = std::span<uint8_t const>(source.data() + offset, size);
    }

    offset += read_bytes;
    return dest;
};

std::string_view ionengine::lib::get_line(std::string_view const buffer, size_t& offset, char const delimeter) {
    
    size_t read_bytes = 0;
    std::string_view line;
    
    for(size_t i = offset; i < buffer.size(); ++i) {
        ++read_bytes;
        if(buffer[i] == delimeter) {
            line = std::string_view(buffer.data() + offset, buffer.data() + i);
            break;
        }
    }

    offset += read_bytes;
    return line;
}

Expected<std::vector<uint8_t>, Result<IOError>> ionengine::lib::load_file(std::filesystem::path const& file_path) {

    std::ifstream ifs(file_path, std::ios::beg | std::ios::binary);

    if(!ifs.is_open()) {
        return lib::make_expected<std::vector<uint8_t>, Result<IOError>>(
            Result<IOError> { 
                .errc = IOError::OpenError,
                .message = "File is not open"
            }
        );
    }

    ifs.seekg(0, std::ios::end);
    size_t total_bytes = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(total_bytes);
    ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    return lib::make_expected<std::vector<uint8_t>, Result<IOError>>(std::move(buffer));
};
