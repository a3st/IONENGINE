// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/context.h>
#include <renderer/cache/geometry.h>

namespace ionengine::scene {
class Scene;
}

namespace ionengine::renderer {

struct Renderable {
    GeometryBuffer* buffer;
    
};

class Renderer {
public:

    Renderer(platform::Window& window);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void render(scene::Scene& scene);

    void resize(uint32_t const width, uint32_t const height);

private:

    Context _context;
    FrameGraph _frame_graph;

    Attachment* _gbuffer_color_buffer;

    Attachment* _swapchain_buffer;

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count);

    std::vector<Renderable> _renderables;
};

}