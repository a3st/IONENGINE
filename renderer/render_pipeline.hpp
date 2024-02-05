// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "camera.hpp"
#include "core/ref_ptr.hpp"
#include "render_graph.hpp"
#include "render_task.hpp"

namespace ionengine::renderer
{
    class RenderPipeline : public core::ref_counted_object
    {
      public:
        RenderPipeline() = default;

        auto initialize(RenderTaskStream* stream) -> void
        {

            this->stream = stream;
        }

        virtual auto setup(RenderGraphBuilder& builder, core::ref_ptr<Camera> camera,
                           core::ref_ptr<rhi::Shader> test_shader) -> void = 0;

      protected:
        RenderTaskStream* stream;
    };
} // namespace ionengine::renderer