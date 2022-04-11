// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/frame_graph.h>

namespace ionengine::platform {
class Window;
}

namespace ionengine::renderer {

class Context {
public:

    Context(platform::Window& window, uint32_t const buffered_frame_count);

    Context(Context const&) = delete;

    Context(Context&&) noexcept = delete;

    Context& operator=(Context const&) = delete;

    Context& operator=(Context&&) noexcept = delete;

    void render();

    FrameGraph& frame_graph();

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count);

private:

    backend::Device _device;

    std::vector<uint64_t> _graphics_fence_values;

    uint32_t _buffered_frame_count{0};
    uint32_t _frame_index{0};

    Attachment* _swapchain_buffer;

    FrameGraph _frame_graph;
};

}