// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_buffer.h>

using namespace ionengine::renderer;

UploadBuffer::UploadBuffer(backend::Handle<backend::CommandList> const& command_list, UploadBufferFlags flags) {


}

void UploadBuffer::upload_data(backend::Device& device, backend::Handle<backend::Buffer> const dest, uint64_t const offset, std::span<uint8_t const> const data) {

    device.upload_buffer_data(_upload_buffer, _offset, data);

    device.barrier(_command_list, dest, backend::MemoryState::Common, backend::MemoryState::CopyDest);
    device.copy_buffer_region(_command_list, dest, offset, _upload_buffer, _offset, data.size());
    device.barrier(_command_list, dest, backend::MemoryState::CopyDest, backend::MemoryState::Common);

    _offset += data.size();

    
}