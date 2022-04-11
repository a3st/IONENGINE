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

    void build_frame_graph();

private:

    backend::Device _device;

    FrameGraph _frame_graph;
};

}