// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

inline size_t read_bytes(std::span<uint8_t const> const source, uint64_t& offset, std::span<uint8_t>& dest, size_t const size) {
    
    size_t read_bytes = 0;

    if(offset + size <= source.size()) {
        read_bytes = size;
        dest = std::span<uint8_t>(const_cast<uint8_t* const>(source.data()) + offset, size);
    }

    offset += read_bytes;

    return read_bytes;
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

inline bool load_bytes_from_file(std::filesystem::path const& file_path, std::span<uint8_t> const dest, std::ios::openmode const ios = std::ios::beg) {

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

inline bool save_bytes_to_file(std::filesystem::path const& file_path, std::span<uint8_t const> const data, std::ios::openmode const open_mode = std::ios::beg) {
    
    std::ofstream ofs(file_path, open_mode);
    
    if(!ofs.is_open()) {
        return false;
    }

    ofs.write(reinterpret_cast<char const*>(data.data()), data.size());

    return true;
};

}
