// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/context.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

Context::Context(platform::Window& window, uint32_t const buffered_frame_count) : 
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = buffered_frame_count }),
    _buffered_frame_count(buffered_frame_count) {

    _graphics_fence_values.resize(buffered_frame_count);

    // std::generate_n(_upload_buffers.begin(), buffered_frame_count, [&] { return UploadBuffer(_device); });
}

void Context::swap_buffers() {

    _device.present();
    _frame_index = (_frame_index + 1) % _buffered_frame_count;
}

backend::Device& Context::device() {

    return _device;
}

backend::Handle<backend::Texture> Context::get_or_wait_previous_frame() {

    backend::Handle<backend::Texture> swapchain_texture = _device.acquire_next_texture();
    //_device.wait(_graphics_fence_values[_frame_index], backend::QueueFlags::Graphics);
    return swapchain_texture;
}

uint64_t Context::upload_buffer_data(backend::Handle<backend::Buffer> const& dest, uint64_t const offset, std::span<uint8_t const> const data, BufferUsage const usage) {

    if(usage == BufferUsage::Dynamic) {

        _device.upload_buffer_data(dest, offset, data);
        return 0;
        
    } else {
        
        
        return 0;
    }
}