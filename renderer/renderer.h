// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <renderer/frontend/frame_graph.h>

namespace ionengine::scene {
class Scene;
}

namespace ionengine::renderer {

class Renderer {
public:

    Renderer(platform::Window& window);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void render(scene::Scene& scene);

    void resize(uint32_t const width, uint32_t const height);

    void load_shader();

private:

    frontend::Context _context;
    frontend::FrameGraph _frame_graph;

    frontend::Attachment* _gbuffer_color_buffer;
    frontend::Attachment* _swapchain_buffer;

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count);

    //std::vector<Renderable> _renderables;

    backend::Handle<backend::Buffer> _buffer_triangle;
};

}