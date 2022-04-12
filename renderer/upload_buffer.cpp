// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_buffer.h>

using namespace ionengine::renderer;

UploadBuffer::UploadBuffer(backend::Device& device, UploadBufferFlags flags)
    : _device(&device) {

    _command_list = device.create_command_list(backend::QueueFlags::Copy);
    _upload_buffer = device.create_buffer(64 * 1024 * 1024, backend::BufferFlags::HostWrite);
}

UploadBuffer::UploadBuffer(UploadBuffer&& other) noexcept {

    _device = other._device;
    _upload_buffer = other._upload_buffer;
    _command_list = other._command_list;
    _offset = other._offset;
}

UploadBuffer& UploadBuffer::operator=(UploadBuffer&& other) noexcept {

    _device = other._device;
    _upload_buffer = other._upload_buffer;
    _command_list = other._command_list;
    _offset = other._offset;
    return *this;
}

UploadBuffer::~UploadBuffer() {

    _device->delete_command_list(_command_list);
    _device->delete_buffer(_upload_buffer);
}

void UploadBuffer::upload_data(backend::Handle<backend::Buffer> const dest, uint64_t const offset, std::span<uint8_t const> const data) {

    _device->upload_buffer_data(_upload_buffer, _offset, data);

    _device->barrier(_command_list, dest, backend::MemoryState::Common, backend::MemoryState::CopyDest);
    _device->copy_buffer_region(_command_list, dest, offset, _upload_buffer, _offset, data.size());
    _device->barrier(_command_list, dest, backend::MemoryState::CopyDest, backend::MemoryState::Common);

    _offset += data.size();
}