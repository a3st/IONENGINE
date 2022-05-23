// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_context.h>

using namespace ionengine;
using namespace ionengine::renderer;

UploadContext::UploadContext(backend::Device& device) :
    _device(&device) {

    _command_list = device.create_command_list(backend::QueueFlags::Copy);
    _buffer = device.create_buffer(1024 * 1024 * 32, backend::BufferFlags::HostWrite);
}

void UploadContext::begin() {
    _offset = 0;
}

void UploadContext::copy_buffer_data(backend::Handle<backend::Buffer> const& dest, uint64_t const offset, std::span<uint8_t const> const data) {

    uint8_t* bytes = _device->map_buffer_data(_buffer, _offset);
    std::memcpy(bytes, data.data(), data.size_bytes());
    _device->unmap_buffer_data(_buffer);

    _device->barrier(_command_list, dest, backend::MemoryState::Common, backend::MemoryState::CopyDest);
    _device->copy_buffer_region(_command_list, dest, offset, _buffer, _offset, data.size_bytes());
    _device->barrier(_command_list, dest, backend::MemoryState::CopyDest, backend::MemoryState::Common);
    _offset += data.size_bytes();
}

void UploadContext::copy_texture_data(
    backend::Handle<backend::Texture> const& dest,
    backend::Format const format,
    uint32_t const width,
    uint32_t const height,
    uint32_t const depth,
    uint32_t const mip_count,
    std::span<uint8_t const> const data
) {
    bool bc = false;
    uint32_t bpe = 0;

    switch(format) {
        case backend::Format::BC1:
        case backend::Format::BC4: {
            bc = true;
            bpe = 8;
        } break;
        case backend::Format::BC5: {
            bc = true;
            bpe = 16;
        } break;
    }

    uint32_t mip_width = width;
    uint32_t mip_height = height;
    uint64_t offset = 0;

    std::vector<backend::TextureCopyRegion> regions;

    uint8_t* bytes = _device->map_buffer_data(_buffer, 0); 
    for(uint32_t i = 0; i  < mip_count; ++i) {

        size_t row_bytes = 0;
        uint32_t row_count = 0;
        uint32_t stride = 4;

        if(bc) {
            uint64_t const block_wide = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_width) + 3u) / 4u);
            uint64_t const block_high = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_height) + 3u) / 4u);

            row_bytes = block_wide * bpe;
            row_count = static_cast<uint32_t>(block_high);
        }

        uint32_t const row_pitch = (mip_width * stride + (256 - 1)) & ~(256 - 1);

        regions.emplace_back(i, row_pitch, static_cast<uint32_t>(_offset));

        uint8_t* dest = bytes + _offset;
        uint8_t const* source = data.data() + offset;

        for(uint32_t i = 0; i < row_count; ++i) {
            std::memcpy(dest + row_pitch * i, source + row_bytes * i, row_bytes);
        }
        
        mip_width = std::max<uint32_t>(1, mip_width / 2);
        mip_height = std::max<uint32_t>(1, mip_height / 2);

        offset += row_bytes * row_count;
        _offset = (_offset + row_count * row_pitch + (512 - 1)) & ~(512 - 1);
    }
    _device->unmap_buffer_data(_buffer);

    _device->barrier(_command_list, dest, backend::MemoryState::Common, backend::MemoryState::CopyDest);
    _device->copy_texture_region(_command_list, dest, _buffer, regions);
    _device->barrier(_command_list, dest, backend::MemoryState::CopyDest, backend::MemoryState::Common);
}

void UploadContext::end() {
    uint64_t fence_value = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&_command_list, 1), backend::QueueFlags::Copy);
    _device->wait(fence_value, backend::QueueFlags::Copy);
}
