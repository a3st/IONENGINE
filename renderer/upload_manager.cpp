// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_manager.h>

using namespace ionengine;
using namespace ionengine::renderer;

void UploadManager::get_texture_data(backend::Format const format, uint32_t const width, uint32_t const height, size_t& row_bytes, uint32_t& row_count) {
    bool bc = false;
    uint32_t bpe = 0;

    switch(format) {
        case backend::Format::BC1:
        case backend::Format::BC4: {
            bc = true;
            bpe = 8;
        } break;
        case backend::Format::BC3:
        case backend::Format::BC5: {
            bc = true;
            bpe = 16;
        } break;
    }

    if(bc) {
        uint64_t const block_wide = std::max<uint64_t>(1u, (static_cast<uint64_t>(width) + 3u) / 4u);
        uint64_t const block_high = std::max<uint64_t>(1u, (static_cast<uint64_t>(height) + 3u) / 4u);
        row_bytes = block_wide * bpe;
        row_count = static_cast<uint32_t>(block_high);
    } else {
        size_t const bpp = 32;
        row_bytes = (width * bpp + 7) / 8;
        row_count = height;
    }
}

UploadManager::UploadManager(backend::Device& device) :
    _device(&device),
    _copy_command_pool(*_device, UPLOAD_MANAGER_DISPATCH_TASK_COUNT),
    _fence_value(0),
    _dispatch_upload_task_count(0),
    _pending_upload_task_count(0) {

    _upload_buffer = std::make_unique<UploadBuffer>(
        device.create_buffer(UPLOAD_MANAGER_BUFFER_SIZE, backend::BufferFlags::HostWrite),
        UPLOAD_MANAGER_BUFFER_SIZE,
        0
    );
}

bool UploadManager::get_required_memory_offset(size_t const size, uint64_t& required_offset) {
    bool result = false;

    uint32_t const resource_alignment_mask = backend::TEXTURE_RESOURCE_ALIGNMENT - 1;
    uint64_t const current_offset = (_upload_buffer->offset + resource_alignment_mask) & ~resource_alignment_mask;

    if(current_offset + size <= _upload_buffer->size) {
        required_offset = current_offset;
        result = true;
    }
    return result;
}

bool UploadManager::upload_texture_data(ResourcePtr<GPUTexture> resource, std::span<uint8_t const> const data, bool const async) {

    if(_pending_upload_task_count.load() == 0) {
        _copy_command_pool.reset();
        _upload_buffer->offset = 0;
        _dispatch_upload_task_count = 0;
    }

    if(_dispatch_upload_task_count == UPLOAD_MANAGER_DISPATCH_TASK_COUNT) {
        return false;
    }

    /*size_t const required_size = _device->get_memory_required_size(resource->get().texture);
    uint64_t required_offset;

    bool result = get_required_memory_offset(data.size_bytes() + 4 * 1024 * 1024, required_offset);

    if(!result) {
        return;
    }

    _upload_buffer->offset += 4 * 1024 * 1024;*/

    if(_upload_buffer->offset + data.size_bytes() + 1024 * 1024 * 4 > _upload_buffer->size) {
        return false;
    }

    ResourcePtr<CommandList> command_list = _copy_command_pool.allocate();

    resource->is_wait_for_upload.store(false);
    GPUTexture gpu_texture = resource->commit_pending();

    uint32_t const row_pitch_alignment_mask = backend::TEXTURE_ROW_PITCH_ALIGNMENT - 1;
    uint32_t const resource_alignment_mask = backend::TEXTURE_RESOURCE_ALIGNMENT - 1;

    uint64_t mip_offset = 0;

    std::vector<backend::TextureCopyRegion> regions;

    uint8_t* buffer_data = _device->map_buffer_data(_upload_buffer->buffer, 0);
    //uint64_t buffer_offset = required_offset;

    for(uint32_t k = 0; k < gpu_texture.depth; ++k) {

        uint32_t mip_width = gpu_texture.width;
        uint32_t mip_height = gpu_texture.height;

        size_t row_bytes;
        uint32_t row_count;
        uint32_t const stride = 4;

        for(uint32_t i = 0; i < gpu_texture.mip_count; ++i) {

            get_texture_data(gpu_texture.format, mip_width, mip_height, row_bytes, row_count);

            uint32_t const row_pitch = (mip_width * stride + row_pitch_alignment_mask) & ~row_pitch_alignment_mask;

            regions.emplace_back(gpu_texture.mip_count * k + i, row_pitch, _upload_buffer->offset);

            uint8_t* dest = buffer_data + _upload_buffer->offset;
            uint8_t const* source = data.data() + mip_offset;

            for(uint32_t j = 0; j < row_count; ++j) {
                std::memcpy(dest + row_pitch * j, source + row_bytes * j, row_bytes);
            }
                    
            mip_width = std::max<uint32_t>(1, mip_width / 2);
            mip_height = std::max<uint32_t>(1, mip_height / 2);

            mip_offset += row_bytes * row_count;
            _upload_buffer->offset = (_upload_buffer->offset + row_count * row_pitch + resource_alignment_mask) & ~resource_alignment_mask;
        }
    }

    _device->unmap_buffer_data(_upload_buffer->buffer);

    lib::thread_pool().push(
        lib::TaskPriorityFlags::High,
        [&, command_list, regions](backend::Handle<backend::Texture> const& texture) {
            auto memory_barrier = backend::MemoryBarrierDesc {
                .target = texture,
                .before = backend::MemoryState::Common,
                .after = backend::MemoryState::CopyDest
            };

            command_list->get().barrier(*_device, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));

            _device->copy_texture_region(command_list->get().command_list, texture, _upload_buffer->buffer, regions);

            memory_barrier = backend::MemoryBarrierDesc {
                .target = texture,
                .before = backend::MemoryState::CopyDest,
                .after = backend::MemoryState::Common
            };

            command_list->get().barrier(*_device, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));
        },
        gpu_texture.texture
    );
    
    auto upload_resource_task_data = UploadTaskData<GPUTexture> {
        .ptr = resource,
        .resource = std::move(gpu_texture)
    };

    _tasks.push_back(
        UploadTask {
            .data = std::move(upload_resource_task_data),
            .command_list = command_list
        }
    );

    ++_pending_upload_task_count;
    ++_dispatch_upload_task_count;
    return true;
}

bool UploadManager::upload_buffer_data(ResourcePtr<GPUBuffer> resource, uint64_t const offset, std::span<uint8_t const> const data, bool const async) {

    if(resource->get().is_host_visible()) {

        uint8_t* buffer_data = _device->map_buffer_data(resource->get().buffer, offset);
        std::memcpy(buffer_data, data.data(), data.size_bytes());
        _device->unmap_buffer_data(resource->get().buffer);

    } else {

        if(_pending_upload_task_count.load() == 0) {
            _copy_command_pool.reset();
            _upload_buffer->offset = 0;
            _dispatch_upload_task_count = 0;
        }

        if(_dispatch_upload_task_count == UPLOAD_MANAGER_DISPATCH_TASK_COUNT) {
            return false;
        }

        size_t const required_size = data.size_bytes();
        uint64_t required_offset;

        bool result = get_required_memory_offset(required_size, required_offset);

        if(!result) {
            return false;
        }

        _upload_buffer->offset += required_size;

        ResourcePtr<CommandList> command_list = _copy_command_pool.allocate();

        resource->is_wait_for_upload.store(false);
        GPUBuffer gpu_buffer = resource->commit_pending();

        uint8_t* buffer_data = _device->map_buffer_data(_upload_buffer->buffer, required_offset);
        std::memcpy(buffer_data, data.data(), data.size_bytes());
        _device->unmap_buffer_data(_upload_buffer->buffer);

        lib::thread_pool().push(
            lib::TaskPriorityFlags::High,
            [&, command_list, required_offset, required_size](backend::Handle<backend::Buffer> const& buffer, uint64_t const offset) {
                auto memory_barrier = backend::MemoryBarrierDesc {
                    .target = buffer,
                    .before = backend::MemoryState::Common,
                    .after = backend::MemoryState::CopyDest
                };
                _device->barrier(command_list->get().command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));

                _device->copy_buffer_region(command_list->get().command_list, buffer, offset, _upload_buffer->buffer, required_offset, required_size);

                memory_barrier = backend::MemoryBarrierDesc {
                    .target = buffer,
                    .before = backend::MemoryState::CopyDest,
                    .after = backend::MemoryState::Common
                };
                _device->barrier(command_list->get().command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));
            },
            gpu_buffer.buffer,
            offset
        );

        auto upload_resource_task_data = UploadTaskData<GPUBuffer> {
            .ptr = resource,
            .resource = std::move(gpu_buffer)
        };

        _tasks.push_back(
            UploadTask {
                .data = std::move(upload_resource_task_data),
                .command_list = command_list
            }
        );

        ++_pending_upload_task_count;
        ++_dispatch_upload_task_count;
    }
    return true;
}

bool UploadManager::upload_geometry_data(ResourcePtr<GeometryBuffer> resource, std::span<uint8_t const> const vertex_data, std::span<uint8_t const> const index_data, bool const async) {

    if(resource->get().is_host_visible()) {

        uint8_t* buffer_data = _device->map_buffer_data(resource->get().vertex_buffer, 0);
        std::memcpy(buffer_data, vertex_data.data(), vertex_data.size_bytes());
        _device->unmap_buffer_data(resource->get().vertex_buffer);

        buffer_data = _device->map_buffer_data(resource->get().index_buffer, 0);
        std::memcpy(buffer_data, index_data.data(), index_data.size_bytes());
        _device->unmap_buffer_data(resource->get().index_buffer);

    } else {

        if(_pending_upload_task_count.load() == 0) {
            _copy_command_pool.reset();
            _upload_buffer->offset = 0;
            _dispatch_upload_task_count = 0;
        }

        if(_dispatch_upload_task_count == UPLOAD_MANAGER_DISPATCH_TASK_COUNT) {
            return false;
        }

        uint32_t const resource_alignment_mask = backend::TEXTURE_RESOURCE_ALIGNMENT - 1;

        size_t const required_size = 
            (vertex_data.size_bytes() + index_data.size_bytes() + resource_alignment_mask) & ~resource_alignment_mask;

        uint64_t required_offset;

        bool result = get_required_memory_offset(required_size, required_offset);

        if(!result) {
            return false;
        }

        _upload_buffer->offset += required_size;

        ResourcePtr<CommandList> command_list = _copy_command_pool.allocate();

        resource->is_wait_for_upload.store(false);
        GeometryBuffer geometry_buffer = resource->commit_pending();

        uint8_t* buffer_data = _device->map_buffer_data(_upload_buffer->buffer);
        uint64_t buffer_offset = required_offset;

        size_t const vertex_data_size = vertex_data.size_bytes();
        uint64_t const vertex_data_offset = buffer_offset;
        std::memcpy(buffer_data + buffer_offset, vertex_data.data(), vertex_data.size_bytes());

        buffer_offset = buffer_offset + vertex_data.size_bytes();
            
        size_t const index_data_size = index_data.size_bytes();
        uint64_t const index_data_offset = buffer_offset;
        std::memcpy(buffer_data + buffer_offset, index_data.data(), index_data.size_bytes());
                
        _device->unmap_buffer_data(_upload_buffer->buffer);

        lib::thread_pool().push(
            lib::TaskPriorityFlags::High,
            [&, command_list, vertex_data_offset, vertex_data_size, index_data_offset, index_data_size](backend::Handle<backend::Buffer> const& vertex_buffer, backend::Handle<backend::Buffer> const& index_buffer) {
                std::array<backend::MemoryBarrierDesc, 2> barriers;

                barriers.at(0) = backend::MemoryBarrierDesc {
                    .target = vertex_buffer,
                    .before = backend::MemoryState::Common,
                    .after = backend::MemoryState::CopyDest
                };
                barriers.at(1) = backend::MemoryBarrierDesc {
                    .target = index_buffer,
                    .before = backend::MemoryState::Common,
                    .after = backend::MemoryState::CopyDest
                };
                _device->barrier(command_list->get().command_list, barriers);

                _device->copy_buffer_region(command_list->get().command_list, vertex_buffer, 0, _upload_buffer->buffer, vertex_data_offset, vertex_data_size);
                
                _device->copy_buffer_region(command_list->get().command_list, index_buffer, 0, _upload_buffer->buffer, index_data_offset, index_data_size);

                barriers.at(0) = backend::MemoryBarrierDesc {
                    .target = vertex_buffer,
                    .before = backend::MemoryState::CopyDest,
                    .after = backend::MemoryState::Common
                };
                barriers.at(1) = backend::MemoryBarrierDesc {
                    .target = index_buffer,
                    .before = backend::MemoryState::CopyDest,
                    .after = backend::MemoryState::Common
                };
                _device->barrier(command_list->get().command_list, barriers);
            },
            geometry_buffer.vertex_buffer,
            geometry_buffer.index_buffer
        );

        auto upload_resource_task_data = UploadTaskData<GeometryBuffer> {
            .ptr = resource,
            .resource = std::move(geometry_buffer)
        };

        _tasks.push_back(
            UploadTask {
                .data = std::move(upload_resource_task_data),
                .command_list = command_list
            }
        );

        ++_pending_upload_task_count;
        ++_dispatch_upload_task_count;
    }
    return true;
}

void UploadManager::dispatch() {

    if(_tasks.empty()) {
        return;
    }

    lib::thread_pool().wait_all(lib::TaskPriorityFlags::High);

    for(auto& task : _tasks) {

        uint64_t const fence_value = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&task.command_list->get().command_list, 1), backend::QueueFlags::Copy);

        auto upload_task_visitor = make_visitor(
            [&](UploadTaskData<GPUBuffer>& data) {

                lib::thread_pool().push(
                    lib::TaskPriorityFlags::Low,
                    [&, fence_value, data]() {
                        GPUBuffer gpu_buffer = data.resource;

                        while(true) {
                            if(_device->is_completed(fence_value, backend::QueueFlags::Copy)) {
                                data.ptr->commit_ok(std::move(gpu_buffer));
                                --_pending_upload_task_count;
                                break;
                            } else {
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            }
                        }
                    }
                );
            },
            [&](UploadTaskData<GPUTexture>& data) {

                lib::thread_pool().push(
                    lib::TaskPriorityFlags::Low,
                    [&, fence_value, data]() {
                        GPUTexture gpu_texture = data.resource;

                        while(true) {
                            if(_device->is_completed(fence_value, backend::QueueFlags::Copy)) {
                                data.ptr->commit_ok(std::move(gpu_texture));
                                --_pending_upload_task_count;
                                break;
                            } else {
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            }
                        }
                    }
                );
            },
            [&](UploadTaskData<GeometryBuffer>& data) {

                lib::thread_pool().push(
                    lib::TaskPriorityFlags::Low,
                    [&, fence_value, data]() {
                        GeometryBuffer geometry_buffer = data.resource;

                        while(true) {
                            if(_device->is_completed(fence_value, backend::QueueFlags::Copy)) {
                                data.ptr->commit_ok(std::move(geometry_buffer));
                                --_pending_upload_task_count;
                                break;
                            } else {
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            }
                        }
                    }
                );
            }
        );

        std::visit(upload_task_visitor, task.data);

        _fence_value = fence_value;
    }

    _tasks.clear();
}
