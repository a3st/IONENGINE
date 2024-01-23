// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/render_pipeline.hpp"

namespace ionengine {

namespace renderer {

class MyRenderPipeline : public RenderPipeline {
public:

    MyRenderPipeline() = default;

    auto setup(
        RenderGraphBuilder& builder, 
        //core::ref_ptr<Camera> camera, 
        uint32_t const width, 
        uint32_t const height
        //std::span<RenderTask> const render_tasks
    ) -> std::vector<RGAttachment> override;
};

}

}