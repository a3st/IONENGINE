
#include <precompiled.h>
#include <engine/dds_loader.h>

using namespace ionengine;

bool DDSLoader::parse(std::span<char8_t> const data) {

    auto read_bytes = [&](std::span<char8_t> const src, uint64_t& offset, std::span<char8_t>& dst, uint64_t const size) -> size_t {
        size_t read_bytes = 0;
        if(offset + size < src.size()) {
            read_bytes = size;
            dst = std::span<char8_t>(src.data() + offset, src.data() + size);
        }
        offset += read_bytes;
        return read_bytes;
    };

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    DDSFileLayout file_layout;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&file_layout.magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    if(file_layout.magic == static_cast<uint32_t>(DDS::Magic)) {
        std::cout << "It's dds!" << std::endl;
    } else {
        std::cout << "It's not dds!" << std::endl;
        return false;
    }

    return true;
}