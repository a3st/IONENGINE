// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "backend.hpp"
#include "render_graph.hpp"
#include "render_pipeline.hpp"
#include "mesh_renderer.hpp"
#include "sprite_renderer.hpp"

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

    auto render() -> void;

    auto resize(uint32_t const width, uint32_t const height) -> void;

    auto get_mesh_renderer() -> MeshRenderer& {

        return mesh_renderer;
    }

private:

    Backend backend;
    core::ref_ptr<RenderPipeline> render_pipeline;
    core::ref_ptr<RenderGraph> render_graph{nullptr};
    MeshRenderer mesh_renderer;
    SpriteRenderer sprite_renderer;
};

}

}