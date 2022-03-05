// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine::renderer;

FrameGraph& FrameGraph::render_pass() {

    return *this;
}

FrameGraph& FrameGraph::build(Backend& backend) {

    return *this;
}

void FrameGraph::reset(Backend& backend) {

    
}

void FrameGraph::execute(Backend& backend) {

    Handle<Texture> texture = backend.get_current_buffer();

    /*if(rpasses[frame_index] == Handle<RenderPass>()) {
        rpasses[frame_index] = _backend->create_render_pass(
            { texture },
            { RenderPassColorDesc { RenderPassLoadOp::Clear, RenderPassStoreOp::Store } },
            {}, {}
        );

        pipelines[frame_index] = _backend->create_pipeline(
            desc_layout,
            MeshData::vertex_declaration,
            shaders,
            RasterizerDesc { FillMode::Solid, CullMode::Back },
            DepthStencilDesc { CompareOp::Always, false },
            BlendDesc { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
            rpasses[frame_index]
        );
    }

    _backend->begin_context(ContextType::Graphics);
    _backend->set_viewport(0, 0, 800, 600);
    _backend->set_scissor(0, 0, 800, 600);
    _backend->barrier(texture, MemoryState::Present, MemoryState::RenderTarget);
    
    std::vector<Color> rtv_clears = { Color(0.2f, 0.1f, 0.3f, 1.0f) };
    _backend->begin_render_pass(rpasses[frame_index], rtv_clears, 0.0f, 0x0);
    _backend->bind_pipeline(pipelines[frame_index]);
    _backend->bind_descriptor_set(descriptor_set);
    _backend->bind_vertex_buffer(0, buffer_vertex, 0);
    _backend->bind_index_buffer(Format::R32, buffer_index, 0);
    _backend->draw_indexed(index_count, 1, 0);
    _backend->end_render_pass();
    _backend->barrier(texture, MemoryState::RenderTarget, MemoryState::Present);
    _backend->end_context();

    _backend->execute_context(ContextType::Graphics);*/

    backend.swap_buffers();
}