// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_context.h>

using namespace ionengine;
using namespace ionengine::renderer;

UploadContext::UploadContext(backend::Device& device) :
    _device(&device) {

    _command_list = device.create_command_list(backend::QueueFlags::Copy);
    _buffer = device.create_buffer(1024 * 1024 * 16, backend::BufferFlags::HostWrite);
}

void UploadContext::begin() {
    _offset = 0;
}

void UploadContext::copy_buffer_data(backend::Handle<backend::Buffer> const& dest, uint64_t const offset, std::span<uint8_t const> const data) {

    _device->map_buffer_data(_buffer, _offset, data);
    _device->barrier(_command_list, dest, backend::MemoryState::Common, backend::MemoryState::CopyDest);
    _device->copy_buffer_region(_command_list, dest, offset, _buffer, _offset, data.size_bytes());
    _device->barrier(_command_list, dest, backend::MemoryState::CopyDest, backend::MemoryState::Common);
    _offset += data.size_bytes();
}

void UploadContext::end() {
    uint64_t fence_value = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&_command_list, 1), backend::QueueFlags::Copy);
    _device->wait(fence_value, backend::QueueFlags::Copy);
}
