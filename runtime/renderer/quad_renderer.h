// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/renderer.h"

namespace ionengine::renderer {

class QuadRenderer : public BaseRenderer {
public:

    QuadRenderer(RenderBackend& render_backend, FrameGraph& frame_graph) : m_render_backend(render_backend), m_frame_graph(frame_graph) {

        
    }

private:

    std::reference_wrapper<RenderBackend> m_render_backend;
    std::reference_wrapper<FrameGraph> m_frame_graph;


};

}