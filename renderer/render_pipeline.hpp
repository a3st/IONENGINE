// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "render_graph.hpp"
#include "camera.hpp"
#include "render_task.hpp"

namespace ionengine {

namespace renderer {

class RenderPipeline : public core::ref_counted_object {
public:

    RenderPipeline() = default;

    virtual auto setup(
        RenderGraphBuilder& builder, 
        core::ref_ptr<Camera> camera,
        RenderTaskStream& render_task_stream,
        core::ref_ptr<rhi::Shader> test_shader
    ) -> std::vector<RGAttachment> = 0;

protected:

    
};

}

}