// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/cache_manager.h>

using namespace ionengine;
using namespace ionengine::renderer;

CacheManager::CacheManager(backend::Device& device, UploadContext& upload_context) :
    _device(&device),
    _shader_program_pool(device, upload_context),
    _geometry_buffer_pool(device, upload_context) {

    /*for(uint32_t i = 0; i < frame_count; ++i) {
        _command_lists.emplace_back(device.create_command_list(backend::QueueFlags::Copy));
        _buffers.emplace_back(device.create_buffer(_buffer_size, backend::BufferFlags::HostWrite), 0);
    }*/
}

CachePtr<ShaderProgram> CacheManager::get_shader_program(asset::Technique& technique) {
    return _shader_program_pool.get(technique);
}

CachePtr<GeometryBuffer> CacheManager::get_geometry_buffer(asset::Surface& surface) {
    return _geometry_buffer_pool.get(surface);
}

void CacheManager::update(float const delta_time) {

}

/*
void ResourceUpload::upload_buffer(backend::Handle<backend::Buffer> const& buffer, uint64_t const offset, std::span<uint8_t const> const data) {

    _device->map_buffer_data(_buffers[_frame_index].buffer, _buffers[_frame_index].offset, data);

    _device->barrier(_command_lists[_frame_index], _buffers[_frame_index].buffer, backend::MemoryState::GenericRead, backend::MemoryState::CopySource);
    _device->barrier(_command_lists[_frame_index], buffer, backend::MemoryState::Common, backend::MemoryState::CopyDest);
    _device->copy_buffer_region(_command_lists[_frame_index], buffer, offset, _buffers[_frame_index].buffer, _buffers[_frame_index].offset, data.size());
    _device->barrier(_command_lists[_frame_index], _buffers[_frame_index].buffer, backend::MemoryState::CopySource, backend::MemoryState::GenericRead);
    _device->barrier(_command_lists[_frame_index], buffer, backend::MemoryState::CopyDest, backend::MemoryState::Common);

    _buffers[_frame_index].offset += data.size();
}

uint64_t ResourceUpload::submit() {

    uint64_t fence_value = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&_command_lists[_frame_index], 1), backend::QueueFlags::Copy);
    _buffers[_frame_index].offset = 0;
    _frame_index = (_frame_index + 1) % _frame_count;
    return fence_value;
}
*/