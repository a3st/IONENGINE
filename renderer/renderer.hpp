// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "backend.hpp"
#include "render_graph.hpp"
#include "render_pipeline.hpp"
#include "upload_manager.hpp"
#include "shader.hpp"
#include "buffer.hpp"
#include "mesh.hpp"
#include "camera.hpp"

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Renderer {
public:

    Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    auto operator=(Renderer const&) -> Renderer& = delete;

    auto operator=(Renderer&&) -> Renderer& = delete;

    auto render(std::span<core::ref_ptr<Camera>> const targets) -> void;

    auto resize(platform::Window const& window, uint32_t const width, uint32_t const height) -> void;

    auto load_shaders(std::span<ShaderData const> const shaders) -> bool;

private:

    Backend backend;
    core::ref_ptr<RenderPipeline> render_pipeline;
    core::ref_ptr<RenderGraph> render_graph{nullptr};
    ShaderCache shader_cache;
    UploadManager upload_manager;
    BufferAllocator<LinearAllocator> mesh_allocator;
    bool is_graph_initialized{false};
    uint32_t width;
    uint32_t height;
};

}

}