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
            upload_buffer.size = 1024 * 1024 * 32;
            upload_buffer.offset = 0;
        }
        _upload_buffers.at(i) = std::move(upload_buffer);
    }

    _fence_values.fill(0);
}

void UploadManager::upload_texture_data(ResourcePtr<GPUTexture> resource, std::span<uint8_t const> const data, bool const async) {

    if(_upload_buffers.at(_buffer_index).offset + data.size_bytes() + 1024 * 1024 * 4 > _upload_buffers.at(_buffer_index).size) {
        return;
    }

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
    for(uint32_t i = 0; i < resource->mip_count; ++i) {

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

        regions.emplace_back(i, row_pitch, upload_buffer.offset);

        uint8_t* dest = bytes + upload_buffer.offset;
        uint8_t const* source = data.data() + offset;

        for(uint32_t j = 0; j < row_count; ++j) {
            std::memcpy(dest + row_pitch * j, source + row_bytes * j, row_bytes);
        }
        
        mip_width = std::max<uint32_t>(1, mip_width / 2);
        mip_height = std::max<uint32_t>(1, mip_height / 2);

        offset += row_bytes * row_count;
        upload_buffer.offset = (upload_buffer.offset + row_count * row_pitch + (backend::TEXTURE_RESOURCE_ALIGNMENT - 1)) & ~(backend::TEXTURE_RESOURCE_ALIGNMENT - 1);
    }
    _device->unmap_buffer_data(upload_buffer.buffer);

    auto barrier = backend::MemoryBarrierDesc {
        .target = resource->texture,
        .before = backend::MemoryState::Common,
        .after = backend::MemoryState::CopyDest
    };
    _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&barrier, 1));

    _device->copy_texture_region(upload_buffer.command_list, resource->texture, upload_buffer.buffer, regions);

    barrier = backend::MemoryBarrierDesc {
        .target = resource->texture,
        .before = backend::MemoryState::CopyDest,
        .after = backend::MemoryState::Common
    };
    _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&barrier, 1));

    _textures.at(_buffer_index).emplace(resource, resource.commit_pending());
}

void UploadManager::upload_buffer_data(ResourcePtr<GPUBuffer> resource, uint64_t const offset, std::span<uint8_t const> const data, bool const async) {

    if(resource->is_host_visible()) {

        uint8_t* bytes = _device->map_buffer_data(resource->buffer, offset);
        std::memcpy(bytes, data.data(), data.size_bytes());
        _device->unmap_buffer_data(resource->buffer);

    } else {

        if(_upload_buffers.at(_buffer_index).offset + data.size_bytes() + 1024 * 1024 * 4 > _upload_buffers.at(_buffer_index).size) {
            return;
        }

        auto& upload_buffer = _upload_buffers.at(_buffer_index);

        uint8_t* bytes = _device->map_buffer_data(upload_buffer.buffer, upload_buffer.offset);
        std::memcpy(bytes, data.data(), data.size_bytes());
        _device->unmap_buffer_data(upload_buffer.buffer);

        auto barrier = backend::MemoryBarrierDesc {
            .target = resource->buffer,
            .before = backend::MemoryState::Common,
            .after = backend::MemoryState::CopyDest
        };
        _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&barrier, 1));

        _device->copy_buffer_region(upload_buffer.command_list, resource->buffer, offset, upload_buffer.buffer, upload_buffer.offset, data.size_bytes());
        upload_buffer.offset += data.size_bytes();

        barrier = backend::MemoryBarrierDesc {
            .target = resource->buffer,
            .before = backend::MemoryState::CopyDest,
            .after = backend::MemoryState::Common
        };
        _device->barrier(upload_buffer.command_list, std::span<backend::MemoryBarrierDesc const>(&barrier, 1));

        _buffers.at(_buffer_index).emplace(resource, resource.commit_pending());
    }
}

void UploadManager::upload_geometry_data(ResourcePtr<GeometryBuffer> resource, std::span<uint8_t const> const vertex_data, std::span<uint8_t const> const index_data, bool const async) {
    
    if(_upload_buffers.at(_buffer_index).offset + vertex_data.size_bytes() + index_data.size_bytes() + 1024 * 1024 * 4 > _upload_buffers.at(_buffer_index).size) {
        return;
    }

    auto& upload_buffer = _upload_buffers.at(_buffer_index);

    uint8_t* bytes = _device->map_buffer_data(upload_buffer.buffer);
    std::memcpy(bytes + upload_buffer.offset, vertex_data.data(), vertex_data.size_bytes());
    uint64_t const vertex_offset = upload_buffer.offset;
    upload_buffer.offset = (upload_buffer.offset + vertex_data.size_bytes() + (backend::TEXTURE_RESOURCE_ALIGNMENT - 1)) & ~(backend::TEXTURE_RESOURCE_ALIGNMENT - 1);
    std::memcpy(bytes + upload_buffer.offset, index_data.data(), index_data.size_bytes());
    uint64_t const index_offset = upload_buffer.offset;
    upload_buffer.offset = (upload_buffer.offset + index_data.size_bytes() + (backend::TEXTURE_RESOURCE_ALIGNMENT - 1)) & ~(backend::TEXTURE_RESOURCE_ALIGNMENT - 1);
    _device->unmap_buffer_data(upload_buffer.buffer);

    std::array<backend::MemoryBarrierDesc, 2> barriers;

    barriers.at(0) = backend::MemoryBarrierDesc {
        .target = resource->vertex_buffer,
        .before = backend::MemoryState::Common,
        .after = backend::MemoryState::CopyDest
    };
    barriers.at(1) = backend::MemoryBarrierDesc {
        .target = resource->index_buffer,
        .before = backend::MemoryState::Common,
        .after = backend::MemoryState::CopyDest
    };
    _device->barrier(upload_buffer.command_list, barriers);

    _device->copy_buffer_region(upload_buffer.command_list, resource->vertex_buffer, 0, upload_buffer.buffer, vertex_offset, vertex_data.size_bytes());
    _device->copy_buffer_region(upload_buffer.command_list, resource->index_buffer, 0, upload_buffer.buffer, index_offset, index_data.size_bytes());

    barriers.at(0) = backend::MemoryBarrierDesc {
        .target = resource->vertex_buffer,
        .before = backend::MemoryState::CopyDest,
        .after = backend::MemoryState::Common
    };
    barriers.at(1) = backend::MemoryBarrierDesc {
        .target = resource->index_buffer,
        .before = backend::MemoryState::CopyDest,
        .after = backend::MemoryState::Common
    };
    _device->barrier(upload_buffer.command_list, barriers);

    _geometry_buffers.at(_buffer_index).emplace(resource, resource.commit_pending());
}

void UploadManager::update() {

    for(uint32_t i = 0; i < 2; ++i) {

        auto& upload_buffer = _upload_buffers.at(i);
        
        if(upload_buffer.is_close) {
            if(_device->is_completed(_fence_values.at(_buffer_index), backend::QueueFlags::Copy)) {

                auto& buffers = _buffers.at(i);
                while(!buffers.empty()) {
                    UploadData<GPUBuffer> upload_data = std::move(buffers.front());
                    buffers.pop();
                    upload_data.ptr.commit_ok(std::move(upload_data.resource));
                }

                auto& textures = _textures.at(i);
                while(!textures.empty()) {
                    UploadData<GPUTexture> upload_data = std::move(textures.front());
                    textures.pop();
                    upload_data.ptr.commit_ok(std::move(upload_data.resource));
                }

                auto geometry_buffers = _geometry_buffers.at(i);
                while(!geometry_buffers.empty()) {
                    UploadData<GeometryBuffer> upload_data = std::move(geometry_buffers.front());
                    geometry_buffers.pop();
                    upload_data.ptr.commit_ok(std::move(upload_data.resource));
                }

                upload_buffer.is_close = false;
            }
        }
    }

    auto& upload_buffer = _upload_buffers.at(_buffer_index);

    if(upload_buffer.offset > 0 && !upload_buffer.is_close) {

        if(_device->is_completed(_fence_values.at(_buffer_index), backend::QueueFlags::Copy)) {

            _fence_values.at(_buffer_index) = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&upload_buffer.command_list, 1), backend::QueueFlags::Copy);
            upload_buffer.is_close = true;
            upload_buffer.offset = 0;
            _buffer_index = (_buffer_index + 1) % 2;
        }
    }
}
