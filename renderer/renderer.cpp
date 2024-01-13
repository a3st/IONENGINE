// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/renderer.hpp"
#include "platform/window.hpp"
#include "core/exception.hpp"

#include "shader.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Renderer::Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window) 
    : backend(window), render_pipeline(render_pipeline) 
{
    std::string shader_code;
    {
        std::ifstream ifs("shaders/quad.wgsl");
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        shader_code.resize(size);
        ifs.read(reinterpret_cast<char* const>(shader_code.data()), size);
    }
    
    auto shader = core::make_ref<Shader>(backend, shader_code);

    render_graph = render_pipeline->render(backend, window);
}

auto Renderer::render() -> void {

    backend.update();

    render_graph->execute(mesh_renderer, sprite_renderer);
}

auto Renderer::resize(uint32_t const width, uint32_t const height) -> void {
    
    backend.recreate_swapchain(width, height);
}
