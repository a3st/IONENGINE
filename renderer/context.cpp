// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/context.h>

using namespace ionengine::renderer;

Context::Context(platform::Window& window, uint32_t const buffered_frame_count) 
    : 
        _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = buffered_frame_count }),
        _buffered_frame_count(buffered_frame_count)
    {

    build_frame_graph(window.client_size().width, window.client_size().height, buffered_frame_count);

    _graphics_fence_values.resize(buffered_frame_count);
}

void Context::render() {

    backend::Handle<backend::Texture> swapchain_texture = _device.acquire_next_texture();

    _device.wait(_graphics_fence_values[_frame_index], backend::QueueFlags::Graphics);

    _frame_graph.bind_attachment(*_swapchain_buffer, swapchain_texture);

    _graphics_fence_values[_frame_index] = _frame_graph.execute(_device);

    _device.present();

    _frame_index = (_frame_index + 1) % _buffered_frame_count;
}

FrameGraph& Context::frame_graph() {

    return _frame_graph;
}

void Context::build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count) {

    _swapchain_buffer = &_frame_graph.add_attachment("swapchain_buffer", backend::MemoryState::Present, backend::MemoryState::Present);

    CreateColorInfo swapchain_info = {
        .attachment = _swapchain_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = Color(0.4f, 0.2f, 0.1f, 1.0f)
    };

    _frame_graph.add_pass(
        RenderPassDesc {
            .name = "present_only_pass",
            .width = width,
            .height = height,
            .color_infos = std::span<CreateColorInfo const>(&swapchain_info, 1)
        },
        RenderPassDefaultFunc
    );

    _frame_graph.build(_device, buffered_frame_count);
}