// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <renderer/frontend/frame_graph.h>
#include <renderer/frontend/shader_program.h>
#include <renderer/frontend/geometry_buffer.h>
#include <asset/asset_manager.h>
#include <lib/event_dispatcher.h>

namespace ionengine {

namespace scene {
class Scene;
}

}

namespace ionengine::renderer {

class Renderer {
public:

    Renderer(platform::Window& window, asset::AssetManager& asset_manager);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void render(scene::Scene& scene);

    void resize(uint32_t const width, uint32_t const height);

    void load_shader();

private:

    asset::AssetManager* _asset_manager;

    std::optional<mpsc::Receiver<asset::AssetEvent<asset::Mesh>>> _meshes_receiver;

    frontend::Context _context;
    frontend::FrameGraph _frame_graph;

    frontend::Attachment* _gbuffer_color_buffer;
    frontend::Attachment* _swapchain_buffer;

    std::optional<frontend::GeometryBuffer> _offscreen_quad;

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count);

    std::vector<backend::VertexInputDesc> vertex_declaration;

    std::optional<frontend::ShaderProgram> _shader_prog;

    std::optional<frontend::ShaderProgram> _shader_prog_2;

    std::unordered_map<uint32_t, backend::Handle<backend::Pipeline>> _pipelines;

    backend::Handle<backend::Sampler> _sampler;

    std::optional<frontend::GeometryBuffer> _geom_triangle;

    std::vector<backend::Handle<backend::Buffer>> _model_buffer;
};

}