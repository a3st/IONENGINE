// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"
#include "render_graph.hpp"
#include "render_pipeline.hpp"

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

    auto render() -> void;

    auto resize(uint32_t const width, uint32_t const height) -> void;

    // auto load_shaders(std::span<ShaderData const> const shaders) -> bool;

    //auto create_camera(CameraProjectionType const projection_type) -> core::ref_ptr<Camera>;

    /*auto add_render_task(
        PrimitiveData const& data, 
        uint32_t const index_count,
        std::string_view const shader_name
    ) -> void;*/

private:

    core::ref_ptr<rhi::Device> device;
    core::ref_ptr<rhi::MemoryAllocator> primitive_allocator{nullptr};
    core::ref_ptr<RenderPipeline> render_pipeline;
    core::ref_ptr<RenderGraph> render_graph{nullptr};
    bool is_graph_initialized{false};
    uint32_t width;
    uint32_t height;
    //std::vector<RenderTask> render_tasks;
};

}

}