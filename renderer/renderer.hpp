// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"
#include "render_graph.hpp"
#include "render_pipeline.hpp"
#include "cache/primitive_cache.hpp"
#include "render_task.hpp"

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Renderer : public core::ref_counted_object {
public:

    Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window);

    auto update(float const dt) -> void;

    auto render() -> void;

    auto resize(uint32_t const width, uint32_t const height) -> void;

    auto add_render_task(RenderTaskData const& data) -> void;

private:

    core::ref_ptr<rhi::Device> device;
    core::ref_ptr<RenderPipeline> render_pipeline;
    core::ref_ptr<RenderGraph> render_graph{nullptr};
    bool is_graph_initialized{false};
    uint32_t width;
    uint32_t height;
    std::vector<RenderTask> render_tasks;
    PrimitiveCache primitive_cache;
    core::ref_ptr<rhi::Shader> test_shader{nullptr};
};

}

}