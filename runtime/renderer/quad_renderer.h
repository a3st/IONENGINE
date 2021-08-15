// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/renderer.h"

namespace ionengine::renderer {

class QuadRenderer : public BaseRenderer {
public:

    QuadRenderer(FrameGraph& frame_graph) : m_frame_graph(frame_graph) {

        struct DepthPassData {
            FrameGraphResource input;
            FrameGraphResource output;
        };

        m_frame_graph.get().add_pass<DepthPassData>("DepthPass",
            [&](RenderPassBuilder& builder, const DepthPassData& data) {
                input = builder.read_static()
            },
            [=](RenderPassResources& resources, const DepthPassData& data, RenderPassContext& context) {
                // execute pass
            }
        );
    }

private:

    std::reference_wrapper<FrameGraph> m_frame_graph;
};

}