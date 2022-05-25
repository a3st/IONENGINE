// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_manager.h>

using namespace ionengine;
using namespace ionengine::renderer;

UploadManager::UploadManager(lib::ThreadPool& thread_pool, backend::Device& device) :
    _thread_pool(&thread_pool), _device(&device) {

    for(uint32_t i = 0; i < UPLOAD_MANAGER_BUFFER_COUNT; ++i) {

        auto upload_buffer = UploadBuffer {};
        {
            upload_buffer.buffer = device.create_buffer(1024 * 1024 * 32, backend::BufferFlags::HostWrite);
            upload_buffer.command_list = device.create_command_list(backend::QueueFlags::Copy);
            upload_buffer.offset = 0;
        }
        _upload_buffers.at(i) = std::move(upload_buffer);
    }
}

void UploadManager::upload_texture_data(ResourcePtr<GPUTexture> resource, std::span<uint8_t const> const data, bool const async) {

    bool bc = false;
    uint32_t bpe = 0;

    switch(resource->format) {
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

    uint32_t mip_width = resource->width;
    uint32_t mip_height = resource->height;
    uint64_t offset = 0;

    std::vector<backend::TextureCopyRegion> regions;

    auto& upload_buffer = _upload_buffers.at(_buffer_index);

    uint8_t* bytes = _device->map_buffer_data(upload_buffer.buffer, 0); 
    for(uint32_t i = 0; i  < resource->mip_count; ++i) {

        size_t row_bytes = 0;
        uint32_t row_count = 0;
        uint32_t stride = 4;

        if(bc) {
            uint64_t const block_wide = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_width) + 3u) / 4u);
            uint64_t const block_high = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_height) + 3u) / 4u);

            row_bytes = block_wide * bpe;
            row_count = static_cast<uint32_t>(block_high);
        }

        uint32_t const row_pitch = (mip_width * stride + (backend::TEXTURE_ROW_PITCH_ALIGNMENT - 1)) & ~(backend::TEXTURE_ROW_PITCH_ALIGNMENT - 1);

        regions.emplace_back(i, row_pitch, static_cast<uint32_t>(upload_buffer.offset));

        uint8_t* dest = bytes + upload_buffer.offset;
        uint8_t const* source = data.data() + offset;

        for(uint32_t i = 0; i < row_count; ++i) {
            std::memcpy(dest + row_pitch * i, source + row_bytes * i, row_bytes);
        }
        
        mip_width = std::max<uint32_t>(1, mip_width / 2);
        mip_height = std::max<uint32_t>(1, mip_height / 2);

        offset += row_bytes * row_count;
        upload_buffer.offset = (upload_buffer.offset + row_count * row_pitch + (backend::TEXTURE_RESOURCE_ALIGNMENT - 1)) & ~(backend::TEXTURE_RESOURCE_ALIGNMENT - 1);
    }
    _device->unmap_buffer_data(upload_buffer.buffer);

    auto memory_barrier = backend::MemoryBarrierDesc {
        .target = resource->texture,
        .before = backend::MemoryState::Common,
        .after = backend::MemoryState::CopyDest
    };
    _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));

    _device->copy_texture_region(upload_buffer.command_list, resource->texture, upload_buffer.buffer, regions);

    memory_barrier = backend::MemoryBarrierDesc {
        .target = resource->texture,
        .before = backend::MemoryState::CopyDest,
        .after = backend::MemoryState::Common
    };
    _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));

    _textures.at(_buffer_index).emplace(resource, resource.commit_pending());
}

void UploadManager::upload_buffer_data(ResourcePtr<GPUBuffer> resource, uint64_t const offset, std::span<uint8_t const> const data, bool const async) {

    if(resource->is_host_visible()) {

        uint8_t* bytes = _device->map_buffer_data(resource->buffer, offset);
        std::memcpy(bytes, data.data(), data.size_bytes());
        _device->unmap_buffer_data(resource->buffer);

    } else {

        auto& upload_buffer = _upload_buffers.at(_buffer_index);

        uint8_t* bytes = _device->map_buffer_data(upload_buffer.buffer, upload_buffer.offset);
        std::memcpy(bytes, data.data(), data.size_bytes());
        _device->unmap_buffer_data(upload_buffer.buffer);

        auto memory_barrier = backend::MemoryBarrierDesc {
            .target = resource->buffer,
            .before = backend::MemoryState::Common,
            .after = backend::MemoryState::CopyDest
        };
        _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));

        _device->copy_buffer_region(upload_buffer.command_list, resource->buffer, offset, upload_buffer.buffer, upload_buffer.offset, data.size_bytes());
        upload_buffer.offset += data.size_bytes();

        memory_barrier = backend::MemoryBarrierDesc {
            .target = resource->buffer,
            .before = backend::MemoryState::CopyDest,
            .after = backend::MemoryState::Common
        };
        _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));

        _buffers.at(_buffer_index).emplace(resource, resource.commit_pending());
    }
}

void UploadManager::upload_geometry_data(ResourcePtr<GeometryBuffer> resource, std::span<uint8_t const> const vertex_data, std::span<uint8_t const> const index_data, bool const async) {
    
    auto& upload_buffer = _upload_buffers.at(_buffer_index);

    uint8_t* bytes = _device->map_buffer_data(upload_buffer.buffer, upload_buffer.offset);
    std::memcpy(bytes, vertex_data.data(), vertex_data.size_bytes());
    std::memcpy(bytes + vertex_data.size_bytes(), index_data.data(), index_data.size_bytes());
    _device->unmap_buffer_data(upload_buffer.buffer);

    std::array<backend::MemoryBarrierDesc, 2> memory_barriers;

    memory_barriers.at(0) = backend::MemoryBarrierDesc {
        .target = resource->vertex_buffer,
        .before = backend::MemoryState::Common,
        .after = backend::MemoryState::CopyDest
    };
    memory_barriers.at(1) = backend::MemoryBarrierDesc {
        .target = resource->index_buffer,
        .before = backend::MemoryState::Common,
        .after = backend::MemoryState::CopyDest
    };
    _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(memory_barriers.data(), memory_barriers.size()));

    _device->copy_buffer_region(upload_buffer.command_list, resource->vertex_buffer, 0, upload_buffer.buffer, upload_buffer.offset, vertex_data.size_bytes());
    upload_buffer.offset += vertex_data.size_bytes();

    _device->copy_buffer_region(upload_buffer.command_list, resource->index_buffer, 0, upload_buffer.buffer, upload_buffer.offset, index_data.size_bytes());
    upload_buffer.offset += index_data.size_bytes();

    memory_barriers.at(0) = backend::MemoryBarrierDesc {
        .target = resource->vertex_buffer,
        .before = backend::MemoryState::CopyDest,
        .after = backend::MemoryState::Common
    };
    memory_barriers.at(1) = backend::MemoryBarrierDesc {
        .target = resource->index_buffer,
        .before = backend::MemoryState::CopyDest,
        .after = backend::MemoryState::Common
    };
    _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(memory_barriers.data(), memory_barriers.size()));

    _geometry_buffers.at(_buffer_index).emplace(resource, resource.commit_pending());
}

void UploadManager::update() {

    if(_device->is_completed(_fence_values.at(_buffer_index), backend::QueueFlags::Copy)) {
        if(!_upload_buffers.at(_buffer_index).is_close) {
            _fence_values.at(_buffer_index) = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&_upload_buffers.at(_buffer_index).command_list, 1), backend::QueueFlags::Copy);
            _upload_buffers.at(_buffer_index).is_close = true;
        } else {
            while(!_buffers.empty()) {
                UploadData<GPUBuffer> upload_data = std::move(_buffers.at(_buffer_index).front());
                _buffers.at(_buffer_index).pop();
                upload_data.ptr.commit_ok(std::move(upload_data.resource));
            }

            while(!_textures.empty()) {
                UploadData<GPUTexture> upload_data = std::move(_textures.at(_buffer_index).front());
                _textures.at(_buffer_index).pop();
                upload_data.ptr.commit_ok(std::move(upload_data.resource));
            }

            while(!_geometry_buffers.empty()) {
                UploadData<GeometryBuffer> upload_data = std::move(_geometry_buffers.at(_buffer_index).front());
                _geometry_buffers.at(_buffer_index).pop();
                upload_data.ptr.commit_ok(std::move(upload_data.resource));
            }
        }
    }
}
