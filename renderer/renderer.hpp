// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "context.hpp"
#include "render_graph.hpp"
#include "render_pipeline.hpp"
#include "shader.hpp"
#include "buffer.hpp"
#include "primitive.hpp"
#include "primitive_cache.hpp"
#include "texture_cache.hpp"
#include "camera.hpp"
#include "render_task.hpp"

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Renderer : public core::ref_counted_object {
public:

    Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    auto operator=(Renderer const&) -> Renderer& = delete;

    auto operator=(Renderer&&) -> Renderer& = delete;

    auto update(float const dt) -> void;

    auto render(std::span<core::ref_ptr<Camera>> const targets) -> void;

    auto resize(uint32_t const width, uint32_t const height) -> void;

    auto load_shaders(std::span<ShaderData const> const shaders) -> bool;

    auto create_camera(CameraProjectionType const projection_type) -> core::ref_ptr<Camera>;

    auto add_render_task(
        PrimitiveData const& data, 
        uint32_t const index_count,
        std::string_view const shader_name
    ) -> void;

private:

    Context context;
    core::ref_ptr<RenderPipeline> render_pipeline;
    core::ref_ptr<RenderGraph> render_graph{nullptr};
    ShaderCache shader_cache;
    BufferAllocator<LinearAllocator> primitive_allocator;
    TextureCache texture_cache;
    PrimitiveCache primitive_cache;
    bool is_graph_initialized{false};
    uint32_t width;
    uint32_t height;
    std::vector<RenderTask> render_tasks;
};

}

}