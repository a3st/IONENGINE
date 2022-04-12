// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/context.h>

using namespace ionengine::renderer;

Context::Context(platform::Window& window, uint32_t const buffered_frame_count) 
    : 
        _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = buffered_frame_count }),
        _buffered_frame_count(buffered_frame_count)
    {

    _graphics_fence_values.resize(buffered_frame_count);

    _sync_upload_buffer.emplace(_device, UploadBufferFlags::Sync);

    std::generate_n(_async_upload_buffers.begin(), buffered_frame_count, [&] { return UploadBuffer(_device, UploadBufferFlags::Async); });
}

void Context::swap_buffers() {

    _device.present();
    _frame_index = (_frame_index + 1) % _buffered_frame_count;
}

UploadBuffer& Context::upload_buffer(UploadBufferFlags const flags) {

    switch(flags) {
        case UploadBufferFlags::Sync: return _sync_upload_buffer.value();
        case UploadBufferFlags::Async: return _async_upload_buffers[_frame_index];
        default: throw lib::Exception("Upload Buffer Flags");
    }
}

void Context::submit_upload_buffers() {

    uint64_t fence_value = _device.submit(
        std::span<backend::Handle<backend::CommandList> const>(&_sync_upload_buffer.value()._command_list, 1), 
        backend::QueueFlags::Copy
    );
}