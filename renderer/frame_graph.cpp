// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine::renderer;

FrameGraph& FrameGraph::external_attachment(uint32_t const id, Format const format, MemoryState const before, MemoryState const after) {

    auto attachment = ExternalAttachment {
        format,
        before,
        after,
        Handle<Texture>()
    };
    _attachments[{id, 0}] = attachment;
    return *this;
}

FrameGraph& FrameGraph::render_pass(uint32_t const id, RenderPassDesc const& desc, std::function<void()> const& func) {

    auto render_pass = RenderPass {
        desc,
        Handle<renderer::RenderPass>(),
        func
    };
    _render_passes[{id, 0}] = render_pass;
    _ops.emplace_back(OpType::RenderPass, id);
    return *this;
}

void FrameGraph::build(Backend& backend) {

    std::vector<Op> temp_ops;
    temp_ops.resize(_ops.size());
    std::memcpy(temp_ops.data(), _ops.data(), sizeof(Op) * _ops.size());

    for(auto& op : _ops) {

        switch(op.first) {
            case OpType::RenderPass: {
                std::cout << "It's render pass" << std::endl;
            } break;
        }
    }
}

void FrameGraph::reset(Backend& backend) {

    
}

void FrameGraph::execute(Backend& backend) {

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
    }*/

    /*backend.bind_pipeline(pipelines[frame_index]);
    backend.bind_descriptor_set(descriptor_set);
    backend.bind_vertex_buffer(0, buffer_vertex, 0);
    backend.bind_index_buffer(Format::R32, buffer_index, 0);
    backend.draw_indexed(index_count, 1, 0);*/

    backend.begin_context(ContextType::Graphics);
    backend.set_viewport(0, 0, 800, 600);
    backend.set_scissor(0, 0, 800, 600);

    for(auto& op : _ops) {
        switch(op.first) {
            case OpType::RenderPass: {

                auto& render_pass = _render_passes[{ op.second, 0 }];
                auto texture_id = render_pass.desc._color_ids[0].first;
                auto texture = std::get<ExternalAttachment>(_attachments[{ texture_id, 0 }]);

                // for
                backend.barrier(texture.target, texture.before, texture.after);

                std::array<Handle<Texture>, 8> textures;
                textures[0] = texture.target;

                std::array<RenderPassColorDesc, 8> color_descs;
                color_descs[0] = { render_pass.desc._color_ids[0].second, RenderPassStoreOp::Store };

                // create render_pass if not created and try cache it
                render_pass.render_pass = backend.create_render_pass(
                    std::span<Handle<Texture>>(textures.data(), render_pass.desc.color_count),
                    std::span<RenderPassColorDesc>(color_descs.data(), render_pass.desc.color_count),
                    Handle<Texture>(),
                    {}
                );

                // render_pass
                backend.begin_render_pass(render_pass.render_pass, std::span<Color>(render_pass.desc._colors.data(), render_pass.desc.color_count), 0.0f, 0x0);
                render_pass.func();
                backend.end_render_pass();

                // for
                backend.barrier(texture.target, texture.after, texture.before);
            } break;
        }
    }

    backend.end_context();

    backend.execute_context(ContextType::Graphics);
    backend.swap_buffers();
}

FrameGraph& FrameGraph::bind_external_attachment(uint32_t const id, Handle<Texture> const& handle) {

    auto& attachment = std::get<ExternalAttachment>(_attachments[{id, 0}]);
    attachment.target = handle;
    return *this;
}