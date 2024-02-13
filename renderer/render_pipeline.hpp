// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "camera.hpp"
#include "core/ref_ptr.hpp"
#include "render_graph.hpp"
#include "render_task.hpp"
#include "shader.hpp"

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

        virtual auto setup(RenderGraphBuilder& builder, core::ref_ptr<Camera> camera) -> void = 0;

        auto add_custom_shader(core::ref_ptr<Shader> shader) -> void
        {
            auto shader_name = shader->get_shader()->get_name();
            assert(shaders.find(std::string(shader_name)) == shaders.end() && "Can't add a duplicate shader");
            shaders.emplace(std::string(shader_name), shader);
        }

      protected:
        RenderTaskStream* stream;
        std::unordered_map<std::string, core::ref_ptr<Shader>> shaders;
    };
} // namespace ionengine::renderer