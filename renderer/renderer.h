// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <renderer/frontend/frame_graph.h>
#include <renderer/frontend/shader_program.h>
#include <renderer/frontend/geometry_buffer.h>

namespace ionengine::scene {
class Scene;
}

namespace ionengine::renderer {

class Renderer {
public:

    Renderer(platform::Window& window);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void render(scene::Scene& scene);

    void resize(uint32_t const width, uint32_t const height);

    void load_shader();

private:

    frontend::Context _context;
    frontend::FrameGraph _frame_graph;

    frontend::Attachment* _gbuffer_color_buffer;
    frontend::Attachment* _swapchain_buffer;

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count);

    backend::Handle<backend::Buffer> _buffer_triangle;

    std::vector<backend::VertexInputDesc> vertex_declaration;

    std::vector<backend::VertexInputDesc> vertex_declaration_offscreen;

    std::optional<frontend::ShaderProgram> _shader_prog;

    std::optional<frontend::ShaderProgram> _shader_prog_2;

    std::unordered_map<uint32_t, backend::Handle<backend::Pipeline>> _pipelines;

    backend::Handle<backend::Buffer> _vertex_buffer;

    backend::Handle<backend::Buffer> offscreen_vertex_buffer;

    backend::Handle<backend::Sampler> _sampler;

    std::optional<frontend::GeometryBuffer> _geom_triangle;
};

}