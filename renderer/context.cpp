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
}

void Context::swap_buffers() {

    _device.present();
    _frame_index = (_frame_index + 1) % _buffered_frame_count;
}
