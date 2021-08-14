// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/renderer.h"

namespace ionengine::renderer {

class QuadRenderer : public BaseRenderer {
public:

    QuadRenderer(RenderContext& render_context, FrameGraph& frame_graph) : m_frame_graph(frame_graph) {

        
    }

private:

    std::reference_wrapper<FrameGraph> m_frame_graph;


};

}