// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using ionengine::Handle;
using namespace ionengine::renderer;

FrameGraph& FrameGraph::attachment(uint32_t const id, Format const format, Extent2D extent) {

    auto attachment = InternalAttachment {
        format,
        extent,
        Handle<Texture>()
    };
    _attachments[{ id, 0 }] = attachment;
    return *this;
}

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

FrameGraph& FrameGraph::render_pass(uint32_t const id, RenderPassDesc const& desc, RenderPassFunc const& func) {

    auto render_pass = RenderPass {
        desc,
        Handle<renderer::RenderPass>(),
        func
    };
    _render_passes[{ id, 0 }] = render_pass;
    _ops.emplace_back(OpType::RenderPass, id);
    return *this;
}

void FrameGraph::build(Backend& backend, uint32_t const flight_frames) {

    _flight_frames = flight_frames;

    for(auto& op : _ops) {

        switch(op.first) {
            case OpType::RenderPass: {

                bool has_external_attachment = false;

                auto& render_pass = _render_passes[{ op.second, 0 }];

                for(uint32_t i = 0; i < render_pass.desc.color_count; ++i) {
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, ExternalAttachment>) {
                            has_external_attachment = true;
                            _external_attachments_ids[render_pass.desc.color_infos[i].first] = op.second;
                            _render_pass_resources[{ op.second, 0 }]._attachments[render_pass.desc.color_infos[i].first] = Handle<Texture>();
                        } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                            _render_pass_resources[{ op.second, 0 }]._attachments[render_pass.desc.color_infos[i].first] = Handle<Texture>();
                        } else {
                            static_assert(always_false_v<T>, "non-exhaustive visitor!");
                        }
                    }, _attachments[{render_pass.desc.color_infos[i].first, 0}]);
                }

                for(uint32_t i = 0; i < flight_frames; ++i) {
                    auto& render_pass = _render_passes[{ op.second, i }];
                    render_pass = _render_passes[{ op.second, 0 }];

                    for(auto& [id, func] : render_pass.desc.color_infos) {
                        _attachments[{ id, i }] = _attachments[{ id, 0 }];
                    }

                    _render_pass_resources[{ op.second, i }] = _render_pass_resources[{ op.second, 0 }];
                }
                
                for(auto& [key, value] : _attachments) {
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, ExternalAttachment>) {
                            
                        } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                            arg.target = backend.create_texture(Dimension::_2D, arg.extent, 1, 1, arg.format, BackendFlags::RenderTarget);
                        } else {
                            static_assert(always_false_v<T>, "non-exhaustive visitor!");
                        }
                    }, value);
                }

                for(uint32_t i = 0; i < flight_frames; ++i) {
                    for(auto& [key, value] : _render_pass_resources[{ op.second, i }]._attachments) {
                        std::visit([&](auto&& arg) {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, ExternalAttachment>) {
                                value = arg.target;
                            } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                                value = arg.target;
                            } else {
                                static_assert(always_false_v<T>, "non-exhaustive visitor!");
                            }
                        }, _attachments[{ key, i }]);
                    }
                }

                if(has_external_attachment) {
                    _external_render_passes_ids.emplace(op.second);
                }

            } break;
        }
    }
}

void FrameGraph::reset(Backend& backend) {

    backend.wait_for_idle_device();

    for(auto& [key, value] : _render_passes) {

        backend.delete_render_pass(value.render_pass);
    }

    _render_passes.clear();

    for(auto& [key, value] : _attachments) {

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, ExternalAttachment>) {
            
            } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                backend.delete_texture(arg.target);
            } else {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        }, value);
    }

    _attachments.clear();
    
    _render_pass_resources.clear();

    _ops.clear();

    _external_render_passes_ids.clear();
    _external_attachments_ids.clear();
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

                // build external render pass
                if(_external_render_passes_ids.find(op.second) != _external_render_passes_ids.end()) {

                    auto& render_pass = _render_passes[{ op.second, _flight_frame_index }];     
                    if(!render_pass.is_compiled) {
                        if(render_pass.render_pass != Handle<renderer::RenderPass>()) {
                            backend.delete_render_pass(render_pass.render_pass);
                            //std::cout << "FrameGraph: RenderPass was deleted" << std::endl;
                        }
                        
                        render_pass.render_pass = create_render_pass(backend, render_pass.desc);
                        render_pass.is_compiled = true;

                        //std::cout << "FrameGraph: RenderPass was created" << std::endl;

                        for(auto& [key, value] : _render_pass_resources[{ op.second, _flight_frame_index }]._attachments) {
                            std::visit([&](auto&& arg) {
                                using T = std::decay_t<decltype(arg)>;
                                if constexpr (std::is_same_v<T, ExternalAttachment>) {
                                    value = arg.target;
                                } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                                    value = arg.target;
                                } else {
                                    static_assert(always_false_v<T>, "non-exhaustive visitor!");
                                }
                            }, _attachments[{ key, _flight_frame_index }]);
                        }

                        //std::cout << "FrameGraph: Resources was cached" << std::endl;
                    }
                }

                auto& render_pass = _render_passes[{ op.second, _flight_frame_index }];

                // barriers
                for(uint32_t i = 0; i < render_pass.desc.color_count; ++i) {
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, ExternalAttachment>) {
                            backend.barrier(arg.target, arg.after, arg.before);
                        } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                            backend.barrier(arg.target, MemoryState::RenderTarget, MemoryState::RenderTarget);
                        } else {
                            static_assert(always_false_v<T>, "non-exhaustive visitor!");
                        }
                    }, _attachments[{render_pass.desc.color_infos[i].first, _flight_frame_index}]);
                }

                // render_pass
                backend.begin_render_pass(render_pass.render_pass, std::span<Color>(render_pass.desc._clear_colors.data(), render_pass.desc.color_count), 0.0f, 0x0);
                render_pass.func(render_pass.render_pass, _render_pass_resources[{ op.second, _flight_frame_index }]);
                backend.end_render_pass();

                // barriers
                for(uint32_t i = 0; i < render_pass.desc.color_count; ++i) {
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, ExternalAttachment>) {
                            backend.barrier(arg.target, arg.before, arg.after);
                        } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                            backend.barrier(arg.target, MemoryState::RenderTarget, MemoryState::RenderTarget);
                        } else {
                            static_assert(always_false_v<T>, "non-exhaustive visitor!");
                        }
                    }, _attachments[{render_pass.desc.color_infos[i].first, _flight_frame_index}]);
                }
            } break;
        }
    }

    backend.end_context();

    backend.execute_context(ContextType::Graphics);
    backend.swap_buffers();

    _flight_frame_index = (_flight_frame_index + 1) % _flight_frames;
}

FrameGraph& FrameGraph::bind_external_attachment(uint32_t const id, Handle<Texture> const& handle) {

    auto& attachment = std::get<ExternalAttachment>(_attachments[{id, _flight_frame_index}]);
    if(attachment.target != handle) {
        attachment.target = handle;
        _render_passes[{_external_attachments_ids[id], _flight_frame_index}].is_compiled = false;
    }
    return *this;
}

Handle<ionengine::renderer::RenderPass> FrameGraph::create_render_pass(Backend& backend, RenderPassDesc const& desc) {

    std::array<Handle<Texture>, 8> colors;
    std::array<RenderPassColorDesc, 8> color_descs;

    for(uint32_t i = 0; i < desc.color_count; ++i) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, ExternalAttachment>) {
                colors[i] = arg.target;
            } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                colors[i] = arg.target;
            } else {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        }, _attachments[{ desc.color_infos[i].first, _flight_frame_index }]);

        color_descs[i] = RenderPassColorDesc { desc.color_infos[i].second, RenderPassStoreOp::Store };
    }

    Handle<Texture> depth_stencil;
    RenderPassDepthStencilDesc depth_stencil_desc;

    if(desc.has_depth_stencil) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, ExternalAttachment>) {
                depth_stencil = arg.target;
            } else if constexpr (std::is_same_v<T, InternalAttachment>) {
                depth_stencil = arg.target;
            } else {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        }, _attachments[{ desc.depth_stencil_info.first, _flight_frame_index }]);

        depth_stencil_desc = RenderPassDepthStencilDesc { 
            desc.depth_stencil_info.second, RenderPassStoreOp::Store,
            desc.depth_stencil_info.second, RenderPassStoreOp::Store 
        };
    }

    return backend.create_render_pass(
        std::span<Handle<Texture>>(colors.data(), desc.color_count),
        std::span<RenderPassColorDesc>(color_descs.data(), desc.color_count),
        depth_stencil,
        depth_stencil_desc
    );
}
