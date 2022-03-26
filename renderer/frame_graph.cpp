// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using ionengine::Handle;
using namespace ionengine::renderer;

FrameGraph& FrameGraph::attachment(AttachmentId const id, backend::Format const format, uint32_t const width, uint32_t const height, backend::TextureFlags const flags) {

    auto attachment = FrameGraph::InternalAttachment {
        format,
        width,
        height,
        flags,
        INVALID_HANDLE(backend::Texture)
    };
    _attachments[{ id, 0 }] = attachment;
    return *this;
}

FrameGraph& FrameGraph::external_attachment(AttachmentId const id, backend::Format const format, backend::MemoryState const before, backend::MemoryState const after) {

    auto attachment = FrameGraph::ExternalAttachment {
        format,
        before,
        after,
        INVALID_HANDLE(backend::Texture)
    };
    _attachments[{ id, 0 }] = attachment;
    return *this;
}

FrameGraph& FrameGraph::render_pass(RenderPassId const id, RenderPassDesc const& render_pass_desc, RenderPassFunc const& func) {

    auto render_pass = FrameGraph::RenderPass {
        render_pass_desc,
        INVALID_HANDLE(backend::RenderPass),
        func
    };
    _render_passes[{ id, 0 }] = render_pass;
    _ops.emplace_back(OpType::RenderPass, id);
    return *this;
}

void FrameGraph::build(backend::Backend& backend, uint32_t const frame_count) {

    _frame_count = frame_count;

    for(uint32_t i = 1; i < frame_count; ++i) {

        for(auto& [key, value] : _attachments) {
            _attachments[{ key.first, i }] = _attachments[{ key.first, 0 }];
        }

        for(auto& [key, value] : _render_passes) {
            _render_passes[{ key.first, i }] = _render_passes[{ key.first, 0 }];
        }

        for(auto& [key, value] : _render_pass_contexts) {
            _render_pass_contexts[{ key.first, i }] = _render_pass_contexts[{ key.first, 0 }];
        }
    }

    for(auto& [key, value] : _attachments) {

        auto attachment_visitor = make_visitor(
            [&](ExternalAttachment& attachment) {
                attachment.target = INVALID_HANDLE(backend::Texture);
                _memory_states[key.first].first = attachment.before;
            },
            [&](InternalAttachment& attachment) {
                attachment.target = backend.create_texture(
                    backend::Dimension::_2D, 
                    attachment.width, 
                    attachment.height, 
                    1, 
                    1, 
                    attachment.format,
                    attachment.flags
                );
                _memory_states[key.first].first = backend::MemoryState::Common;
            }
        );

        std::visit(attachment_visitor, value);
    }

    for(uint32_t i = 0; i < frame_count; ++i) {
        for(auto& op : _ops) {
            switch(op.first) {
                case FrameGraph::OpType::RenderPass: {
                    auto& render_pass = _render_passes[{ op.second, i }];
                    
                    bool is_external_render_pass = false;

                    for(uint32_t i = 0; i < render_pass.desc.color_count; ++i) {
                        AttachmentId attachment_id = render_pass.desc.color_infos[i].first;

                        auto attachment_visitor = make_visitor(
                            [&](ExternalAttachment& attachment) {
                                is_external_render_pass = true;
                                _external_attachments[attachment_id].emplace(op.second);
                                _render_pass_contexts[{ op.second, i }]._attachments[attachment_id] = INVALID_HANDLE(backend::Texture);
                            },
                            [&](InternalAttachment& attachment) {
                                _render_pass_contexts[{ op.second, i }]._attachments[attachment_id] = attachment.target;
                            }
                        );

                        std::visit(attachment_visitor, _attachments[{ attachment_id, i }]);

                        if(_memory_states[attachment_id].first != backend::MemoryState::RenderTarget) {
                            _memory_states[attachment_id].first = backend::MemoryState::RenderTarget;
                            _memory_states[attachment_id].second.emplace(op.second);
                        }
                    }

                    if(render_pass.desc.has_depth_stencil) {
                        AttachmentId attachment_id = render_pass.desc.depth_stencil_info.first;

                        auto attachment_visitor = make_visitor(
                            [&](ExternalAttachment& attachment) {
                                is_external_render_pass = true;
                                _external_attachments[attachment_id].emplace(op.second);
                                _render_pass_contexts[{ op.second, i }]._attachments[attachment_id] = INVALID_HANDLE(backend::Texture);
                            },
                            [&](InternalAttachment& attachment) {
                                _render_pass_contexts[{ op.second, i }]._attachments[attachment_id] = attachment.target;
                            }
                        );

                        std::visit(attachment_visitor, _attachments[{ attachment_id, i }]);

                        if(_memory_states[attachment_id].first != backend::MemoryState::DepthWrite) {
                            _memory_states[attachment_id].first = backend::MemoryState::DepthWrite;
                            _memory_states[attachment_id].second.emplace(op.second);
                        }
                    }

                    for(uint32_t i = 0; i < static_cast<uint32_t>(render_pass.desc.inputs.size()); ++i) {
                        AttachmentId attachment_id = render_pass.desc.inputs[i];

                        auto attachment_visitor = make_visitor(
                            [&](ExternalAttachment& attachment) {
                                is_external_render_pass = true;
                                _external_attachments[attachment_id].emplace(op.second);
                                _render_pass_contexts[{ op.second, i }]._attachments[attachment_id] = INVALID_HANDLE(backend::Texture);
                            },
                            [&](InternalAttachment& attachment) {
                                _render_pass_contexts[{ op.second, i }]._attachments[attachment_id] = attachment.target;
                            }
                        );

                        std::visit(attachment_visitor, _attachments[{ attachment_id, i }]);

                        if(_memory_states[attachment_id].first != backend::MemoryState::ShaderRead) {
                            _memory_states[attachment_id].first = backend::MemoryState::ShaderRead;
                            _memory_states[attachment_id].second.emplace(op.second);
                        }
                    }

                    if(is_external_render_pass) {
                        _external_render_passes.emplace(op.second);
                    } else {
                        render_pass.render_pass = create_render_pass(backend, i, render_pass.desc);
                    }
                } break;
                case FrameGraph::OpType::ComputePass: {


                } break; 
            }
        }
    }

    for(auto& [key, value] : _attachments) {

        auto attachment_visitor = make_visitor(
            [&](ExternalAttachment& attachment) {
                _memory_states[key.first].first = attachment.before;
            },
            [&](InternalAttachment& attachment) {
                _memory_states[key.first].first = backend::MemoryState::Common;
            }
        );

        std::visit(attachment_visitor, value);
    }
}

void FrameGraph::reset(backend::Backend& backend) {

    backend.wait_for_idle(backend::EncoderFlags::Graphics);

    for(auto& [key, value] : _render_passes) {
        backend.delete_render_pass(value.render_pass);
    }

    _render_passes.clear();
    _render_pass_contexts.clear();

    for(auto& [key, value] : _attachments) {

        auto attachment_visitor = make_visitor(
            [&](ExternalAttachment& attachment) {
                
            },
            [&](InternalAttachment& attachment) {
                backend.delete_texture(attachment.target);
            }
        );

        std::visit(attachment_visitor, value);
    }

    _attachments.clear();

    _external_render_passes.clear();
    _external_attachments.clear();

    _ops.clear();
}

backend::FenceResultInfo FrameGraph::execute(backend::Backend& backend, backend::Encoder& encoder) {

    uint32_t pass_index = 0;

    for(auto& op : _ops) {
        switch(op.first) {
            case OpType::RenderPass: {

                auto& render_pass = _render_passes[{ op.second, _frame_index }];

                // Build external render pass
                if(_external_render_passes.find(op.second) != _external_render_passes.end()) {

                    if(!render_pass.is_compiled) {

                        if(render_pass.render_pass != INVALID_HANDLE(backend::RenderPass)) {
                            backend.delete_render_pass(render_pass.render_pass);
                        }
                        
                        render_pass.render_pass = create_render_pass(backend, _frame_index, render_pass.desc);

                        for(auto& [key, value] : _render_pass_contexts[{ op.second, _frame_index }]._attachments) {

                            auto attachment_visitor = make_visitor(
                                [&](ExternalAttachment& attachment) {
                                    value = attachment.target;
                                },
                                [&](InternalAttachment& attachment) {
                                    value = attachment.target;
                                }
                            );

                            std::visit(attachment_visitor, _attachments[{ key, _frame_index }]);
                        }

                        render_pass.is_compiled = true;
                    }
                }

                encoder
                    .set_viewport(0, 0, render_pass.desc.width, render_pass.desc.height)
                    .set_scissor(0, 0, render_pass.desc.width, render_pass.desc.height);

                // Color Barriers
                for(uint32_t i = 0; i < render_pass.desc.color_count; ++i) {

                    AttachmentId attachment_id = render_pass.desc.color_infos[i].first;

                    if(_memory_states[attachment_id].second.find(op.second) != _memory_states[attachment_id].second.end()) {

                        auto attachment_visitor = make_visitor(
                            [&](ExternalAttachment const& attachment) {
                                encoder.barrier(attachment.target, _memory_states[attachment_id].first, backend::MemoryState::RenderTarget);
                                _memory_states[attachment_id].first = backend::MemoryState::RenderTarget;
                            },
                            [&](InternalAttachment const& attachment) {
                                //std::cout << std::format("(frame {}, pass_index {}) resource acquire ({}) before {}, after {}", _flight_frame_index, pass_index, attachment.target.id, (uint32_t)_memory_states[attachment_id].first, (uint32_t)MemoryState::RenderTarget) << std::endl;
                                encoder.barrier(attachment.target, _memory_states[attachment_id].first, backend::MemoryState::RenderTarget);
                                _memory_states[attachment_id].first = backend::MemoryState::RenderTarget;
                            }
                        );

                        std::visit(attachment_visitor, _attachments.at({ attachment_id, _frame_index }));
                    }
                }

                // Depth Stencil Barriers
                AttachmentId attachment_id = render_pass.desc.depth_stencil_info.first;
                
                if(_memory_states[attachment_id].second.find(op.second) != _memory_states[attachment_id].second.end()) {

                    auto attachment_visitor = make_visitor(
                        [&](ExternalAttachment& attachment) {
                            encoder.barrier(attachment.target, _memory_states[attachment_id].first, backend::MemoryState::DepthWrite);
                            _memory_states[attachment_id].first = backend::MemoryState::DepthWrite;
                        },
                        [&](InternalAttachment& attachment) {
                            encoder.barrier(attachment.target, _memory_states[attachment_id].first, backend::MemoryState::DepthWrite);
                            _memory_states[attachment_id].first = backend::MemoryState::DepthWrite;
                        }
                    );

                    std::visit(attachment_visitor, _attachments[{ attachment_id, _frame_index }]);
                }

                // Input Barriers
                for(uint32_t i = 0; i < static_cast<uint32_t>(render_pass.desc.inputs.size()); ++i) {

                    AttachmentId attachment_id = render_pass.desc.inputs[i];

                    if(_memory_states[attachment_id].second.find(op.second) != _memory_states[attachment_id].second.end()) {

                        auto attachment_visitor = make_visitor(
                            [&](ExternalAttachment& attachment) {
                                encoder.barrier(attachment.target, _memory_states[attachment_id].first, backend::MemoryState::ShaderRead);
                                _memory_states[attachment_id].first = backend::MemoryState::ShaderRead;
                            },
                            [&](InternalAttachment& attachment) {
                                encoder.barrier(attachment.target, _memory_states[attachment_id].first, backend::MemoryState::ShaderRead);
                                _memory_states[attachment_id].first = backend::MemoryState::ShaderRead;
                            }
                        );

                        std::visit(attachment_visitor, _attachments[{ attachment_id, _frame_index }]);
                    }
                }

                encoder.begin_render_pass(
                    render_pass.render_pass, 
                    std::span<Color>(render_pass.desc.clear_colors.data(), render_pass.desc.color_count), 
                    render_pass.desc.clear_depth,
                    render_pass.desc.clear_stencil
                );

                auto& context = _render_pass_contexts[{ op.second, _frame_index }];
                context._render_pass = render_pass.render_pass;
                context._encoder = &encoder;
                context._pass_index = pass_index;
                context._frame_index = _frame_index;

                render_pass.func(context);

                encoder.end_render_pass();

                ++pass_index;
            } break;
        }
    }

    // Final Barriers
    for(auto& [key, value] : _attachments) {

        if(key.second != _frame_index) {
            continue;
        }

        assert(_memory_states[key.first].first != backend::MemoryState::Common && "error during framegraph execution. there are unused resources");
        
        auto attachment_visitor = make_visitor(
            [&](ExternalAttachment& attachment) {
                encoder.barrier(attachment.target, _memory_states[key.first].first, attachment.after);
                _memory_states[key.first].first = attachment.after;
            },
            [&](InternalAttachment& attachment) {
                //std::cout << std::format("(frame {}) resource release ({}) before {}, after {}", _flight_frame_index, attachment.target.id, (uint32_t)_memory_states[key.first].first, (uint32_t)MemoryState::Common) << std::endl;
                encoder.barrier(attachment.target, _memory_states[key.first].first, backend::MemoryState::Common);
                _memory_states[key.first].first = backend::MemoryState::Common;
            }
        );

        std::visit(attachment_visitor, value);
    }

    backend::FenceResultInfo result_info = backend.submit(std::span<backend::Encoder const>(&encoder, 1), backend::EncoderFlags::Graphics);
    backend.present();

    _frame_index = (_frame_index + 1) % _frame_count;

    return result_info;
}

FrameGraph& FrameGraph::bind_external_attachment(AttachmentId const id, Handle<backend::Texture> const& target) {

    auto& attachment = std::get<ExternalAttachment>(_attachments[{id, _frame_index}]);
    if(attachment.target != target) {
        attachment.target = target;
        for(auto& _id : _external_attachments[id]) {
            _render_passes[{ _id, _frame_index }].is_compiled = false;
        }
    }
    return *this;
}

Handle<backend::RenderPass> FrameGraph::create_render_pass(backend::Backend& backend, uint32_t const frame_index, RenderPassDesc const& desc) {

    std::array<Handle<backend::Texture>, 8> colors;
    std::array<backend::RenderPassColorDesc, 8> color_descs;

    for(uint32_t i = 0; i < desc.color_count; ++i) {

        auto attachment_visitor = make_visitor(
            [&](ExternalAttachment& attachment) {
                colors[i] = attachment.target;
            },
            [&](InternalAttachment& attachment) {
                colors[i] = attachment.target;
            }
        );

        std::visit(attachment_visitor, _attachments[{ desc.color_infos[i].first, frame_index }]);

        auto color_desc = backend::RenderPassColorDesc {};
        color_desc.load_op = desc.color_infos[i].second;
        color_desc.store_op = backend::RenderPassStoreOp::Store;

        color_descs[i] = color_desc;
    }

    Handle<backend::Texture> depth_stencil;
    auto depth_stencil_desc = backend::RenderPassDepthStencilDesc {};

    if(desc.has_depth_stencil) {

        auto attachment_visitor = make_visitor(
            [&](ExternalAttachment& attachment) {
                depth_stencil = attachment.target;
            },
            [&](InternalAttachment& attachment) {
                depth_stencil = attachment.target;
            }
        );

        std::visit(attachment_visitor, _attachments[{ desc.depth_stencil_info.first, frame_index }]);

        depth_stencil_desc.depth_load_op = desc.depth_stencil_info.second;
        depth_stencil_desc.depth_store_op = backend::RenderPassStoreOp::Store;
        depth_stencil_desc.stencil_load_op = desc.depth_stencil_info.second;
        depth_stencil_desc.stencil_store_op = backend::RenderPassStoreOp::Store;
    }

    return backend.create_render_pass(
        std::span<Handle<backend::Texture>>(colors.data(), desc.color_count),
        std::span<backend::RenderPassColorDesc>(color_descs.data(), desc.color_count),
        depth_stencil,
        depth_stencil_desc
    );
}
