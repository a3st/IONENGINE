// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/base/window.h"

#include "renderer/api.h"
#include "renderer/render_backend.h"
#include "renderer/frame_graph.h"
#include "renderer/quad_renderer.h"

#include "lib/memory.h"

namespace ionengine {

using namespace memory_literals;

class RenderSystem {
public:

    RenderSystem(platform::Window& window) {

        m_render_backend = std::make_unique<renderer::RenderBackend>(window);
        m_frame_graph = std::make_unique<renderer::FrameGraph>(2);

        // Renderer class
        m_renderer = std::make_unique<renderer::QuadRenderer>(*m_render_backend, *m_frame_graph);
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    void tick() {
        
    }

private:

    std::unique_ptr<renderer::RenderBackend> m_render_backend;
    std::unique_ptr<renderer::FrameGraph> m_frame_graph;

    std::unique_ptr<renderer::BaseRenderer> m_renderer;
};

}