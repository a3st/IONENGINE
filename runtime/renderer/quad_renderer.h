// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "api.h"
#include "base_renderer.h"
#include "frame_graph.h"

namespace ionengine::renderer {

class QuadRenderer : public BaseRenderer {
public:

    QuadRenderer() {

    }

private:

    FrameGraph m_frame_graph;
};

}