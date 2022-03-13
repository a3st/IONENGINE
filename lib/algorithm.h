// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

inline size_t read_bytes(std::span<char8_t const> const source, uint64_t& offset, std::span<char8_t>& dest, size_t const size) {
    
    size_t read_bytes = 0;

    if(offset + size <= source.size()) {
        read_bytes = size;
        dest = std::span<char8_t>(const_cast<char8_t* const>(source.data()) + offset, size);
    }

    offset += read_bytes;

    return read_bytes;
};

inline size_t get_file_size(std::filesystem::path const& file_path, std::ios::openmode const ios = std::ios::beg) {

    std::ifstream ifs(file_path, ios);

    if(!ifs.is_open()) {
        return 0;
    }

    ifs.seekg(0, std::ios::end);
    size_t total_bytes = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    return total_bytes;
}

inline bool load_bytes_from_file(std::filesystem::path const& file_path, std::span<char8_t> const dest, std::ios::openmode const ios = std::ios::beg) {

    std::ifstream ifs(file_path, ios);

    if(!ifs.is_open()) {
        return false;
    }

    ifs.seekg(0, std::ios::end);
    size_t total_bytes = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if(total_bytes > dest.size()) {
        return false;
    }

    ifs.read(reinterpret_cast<char*>(dest.data()), total_bytes);

    return true;
};

inline bool save_bytes_to_file(std::filesystem::path const& file_path, std::span<char8_t const> const data, std::ios::openmode const open_mode = std::ios::beg) {
    
    std::ofstream ofs(file_path, open_mode);
    
    if(!ofs.is_open()) {
        return false;
    }

    ofs.write(reinterpret_cast<char const*>(data.data()), data.size());

    return true;
};

}