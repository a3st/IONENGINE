// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/expected.h>

namespace ionengine::lib {

inline std::span<uint8_t const> read_bytes(std::span<uint8_t const> const source, uint64_t& offset, size_t const size) {
    
    size_t read_bytes = 0;
    std::span<uint8_t const> dest;

    if(offset + size <= source.size()) {
        read_bytes = size;
        dest = std::span<uint8_t const>(source.data() + offset, size);
    }

    offset += read_bytes;
    return dest;
};

inline size_t get_line(std::string_view const buffer, size_t& offset, std::string_view& line, char const delimeter) {
    
    size_t read_bytes = 0;
    
    for(size_t i = offset; i < buffer.size(); ++i) {
        ++read_bytes;
        if(buffer[i] == delimeter) {
            line = std::string_view(buffer.data() + offset, buffer.data() + i);
            break;
        }
    }

    if(read_bytes + offset == buffer.size()) {
        line = std::string_view(buffer.data() + offset, buffer.data() + buffer.size());
    }

    offset += read_bytes;

    return read_bytes;
}

inline size_t get_file_size(std::filesystem::path const& file_path) {

    std::ifstream ifs(file_path, std::ios::beg | std::ios::binary);

    if(!ifs.is_open()) {
        return 0;
    }

    ifs.seekg(0, std::ios::end);
    size_t total_bytes = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    return total_bytes;
}

enum class IOError {
    OpenError
};

inline Expected<std::vector<uint8_t>, Result<IOError>> load_file(std::filesystem::path const& file_path) {

    std::ifstream ifs(file_path, std::ios::beg | std::ios::binary);

    if(!ifs.is_open()) {
        return Expected<std::vector<uint8_t>, Result<IOError>>::error(
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

    return Expected<std::vector<uint8_t>, Result<IOError>>::ok(std::move(buffer));
};

inline bool save_bytes_to_file(std::filesystem::path const& file_path, std::span<uint8_t const> const data, std::ios::openmode const open_mode = std::ios::beg) {
    
    std::ofstream ofs(file_path, open_mode);
    
    if(!ofs.is_open()) {
        return false;
    }

    ofs.write(reinterpret_cast<char const*>(data.data()), data.size());

    return true;
};

}
