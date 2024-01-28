// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"
#include "platform/window.hpp"
#include "render_graph.hpp"
#include "render_pipeline.hpp"
#include "cache/primitive_cache.hpp"
#include "render_task.hpp"
#include "camera.hpp"
#include "render_target.hpp"

namespace ionengine {

namespace renderer {

class Renderer : public core::ref_counted_object {
public:

    Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window);

    auto update(float const dt) -> void;

    auto render(std::span<core::ref_ptr<Camera>> const targets) -> void;

    auto resize(uint32_t const width, uint32_t const height) -> void;

    auto add_render_tasks(RenderTaskData const& data) -> void;

    auto create_render_target(uint32_t const width, uint32_t const height) -> core::ref_ptr<RenderTarget>;

    auto create_camera(
        CameraProjectionType const projection_type, 
        uint32_t const resolution_width,
        uint32_t const resolution_height
    ) -> core::ref_ptr<Camera>;

private:

    core::ref_ptr<rhi::Device> device;
    core::ref_ptr<RenderPipeline> render_pipeline;
    
    core::ref_ptr<rhi::MemoryAllocator> resource_allocator{nullptr};
    core::ref_ptr<PrimitiveCache> primitive_cache{nullptr};

    core::ref_ptr<RenderGraph> render_graph{nullptr};
    bool is_graph_initialized{false};
    uint32_t width;
    uint32_t height;

    std::vector<RenderTask> render_tasks;
    core::ref_ptr<rhi::Shader> test_shader{nullptr};
};

}

}