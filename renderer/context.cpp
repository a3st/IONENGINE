// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/context.h>

using namespace ionengine::renderer;

Context::Context(platform::Window& window, uint32_t const buffered_frame_count) 
    : _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = buffered_frame_count }) {

    
}

void Context::render() {


}

FrameGraph& Context::frame_graph() {

    return _frame_graph;
}