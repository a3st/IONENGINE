// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine::renderer;

Attachment& FrameGraph::add_attachment(AttachmentDesc const& attachment_desc) {

    _attachment_barriers[static_cast<uint32_t>(_attachments.size())].state = backend::MemoryState::Common;
    _attachments.push_back(std::make_unique<Attachment>(attachment_desc));
    return *_attachments.back().get();
}

Attachment& FrameGraph::add_attachment(std::string_view const name, backend::MemoryState const before, backend::MemoryState const after) {

    _attachment_barriers[static_cast<uint32_t>(_attachments.size())].state = before;
    _attachments.push_back(std::make_unique<Attachment>(name, before, after));
    return *_attachments.back().get();
}

RenderPass& FrameGraph::add_pass(RenderPassDesc const& render_pass_desc, RenderPassFunc const& func) {

    _ops.emplace_back(OpType::RenderPass, static_cast<uint32_t>(_render_passes.size()));
    _render_passes.push_back(std::make_unique<RenderPass>(render_pass_desc, func));
    return *_render_passes.back().get();
}

void FrameGraph::build(backend::Device& device, uint32_t const frame_count) {

    _frame_count = frame_count;

    for(size_t i = 0; i < _attachments.size(); ++i) {
        _attachments[i]->_attachments.resize(frame_count);

        for(uint32_t j = 0; j < frame_count; ++j) {

            if(_attachments[i]->is_persistent()) {
                _attachments[i]->_attachments[j] = backend::InvalidHandle<backend::Texture>();
            } else {
                _attachments[i]->_attachments[j] = device.create_texture(
                    backend::Dimension::_2D, 
                    _attachments[i]->_width, 
                    _attachments[i]->_height, 
                    1, 
                    1, 
                    _attachments[i]->_format,
                    _attachments[i]->_flags
                );
            }
        }
    }

    for(auto& op : _ops) {

        switch(op.op_type) {
            case OpType::RenderPass: {
                auto& render_pass = _render_passes[op.index];

                bool is_external_render_pass = false;

                for(auto& attachment : render_pass->_color_attachments) {
                    if(attachment->is_persistent()) {
                        is_external_render_pass = true;
                        break;
                    }

                    if(_attachment_barriers[attachment->hash()].state != backend::MemoryState::RenderTarget) {
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::RenderTarget;
                        _attachment_barriers[attachment->hash()].switch_index.emplace(render_pass->hash());
                    }
                }

                if(render_pass->_depth_stencil_attachment) {
                    if(render_pass->_depth_stencil_attachment->is_persistent()) {
                        is_external_render_pass = true;
                    }

                    if(_attachment_barriers[render_pass->_depth_stencil_attachment->hash()].state != backend::MemoryState::DepthWrite) {
                        _attachment_barriers[render_pass->_depth_stencil_attachment->hash()].state = backend::MemoryState::DepthWrite;
                        _attachment_barriers[render_pass->_depth_stencil_attachment->hash()].switch_index.emplace(render_pass->hash());
                    }
                }

                for(auto& attachment : render_pass->_input_attachments) {
                    if(attachment->is_persistent()) {
                        is_external_render_pass = true;
                        break;
                    }

                    if(_attachment_barriers[attachment->hash()].state != backend::MemoryState::ShaderRead) {
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::ShaderRead;
                        _attachment_barriers[attachment->hash()].switch_index.emplace(render_pass->hash());
                    }
                }

                if(!is_external_render_pass) {
                    for(uint32_t i = 0; i < frame_count; ++i) {
                        compile_render_pass(device, *render_pass, i);
                    }
                }

            } break;

            case OpType::ComputePass: {

            } break;
        }
    }

    for(size_t i = 0; i < _attachments.size(); ++i) {
        if(_attachments[i]->is_persistent()) {
            _attachment_barriers[_attachments[i]->hash()].state = _attachments[i]->_before;
        } else {
            _attachment_barriers[_attachments[i]->hash()].state = backend::MemoryState::Common;
        }
    }
}

void FrameGraph::reset(backend::Device& device) {

    device.wait_for_idle(backend::QueueFlags::Graphics);

    for(auto& render_pass : _render_passes) {
        for(uint32_t i = 0; i < _frame_count; ++i) {
            device.delete_render_pass(render_pass->_render_passes[i]);
        }
    }

    _render_passes.clear();

    for(auto& attachment : _attachments) {
        for(uint32_t i = 0; i < _frame_count; ++i) {
            if(!attachment->is_persistent()) {
                device.delete_texture(attachment->_attachments[i]);
            }
        }
    }

    _attachments.clear();
    _attachment_barriers.clear();

    _ops.clear();
}

uint64_t FrameGraph::execute(backend::Device& device, backend::Handle<backend::CommandList> const& command_list) {

    uint32_t pass_index = 0;

    for(auto& op : _ops) {
        switch(op.op_type) {
            case OpType::RenderPass: {

                auto& render_pass = _render_passes[op.index];

                if(!render_pass->is_compiled()) {

                    if(render_pass->_render_passes[_frame_index] != backend::InvalidHandle<backend::RenderPass>()) {
                        device.delete_render_pass(render_pass->_render_passes[_frame_index]);
                    }

                    compile_render_pass(device, *render_pass, _frame_index);

                    render_pass->_is_compiled = true;
                }

                device.set_viewport(command_list, 0, 0, render_pass->_width, render_pass->_height);
                device.set_scissor(command_list, 0, 0, render_pass->_width, render_pass->_height);

                // Color Barriers
                for(auto& attachment : render_pass->_color_attachments) {

                    if(_attachment_barriers[attachment->hash()].switch_index.find(render_pass->hash()) != _attachment_barriers[attachment->hash()].switch_index.end()) {
                        device.barrier(command_list, attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, backend::MemoryState::RenderTarget);
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::RenderTarget;
                    }
                }

                // Depth Stencil Barrier
                {
                    auto& attachment = render_pass->_depth_stencil_attachment;

                    if(_attachment_barriers[attachment->hash()].switch_index.find(render_pass->hash()) != _attachment_barriers[attachment->hash()].switch_index.end()) {
                        device.barrier(command_list, attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, backend::MemoryState::DepthWrite);
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::DepthWrite;
                    }
                }

                // Input Barriers
                for(auto& attachment : render_pass->_input_attachments) {

                    if(_attachment_barriers[attachment->hash()].switch_index.find(render_pass->hash()) != _attachment_barriers[attachment->hash()].switch_index.end()) {
                        device.barrier(command_list, attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, backend::MemoryState::ShaderRead);
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::ShaderRead;
                    }
                }

                device.begin_render_pass(
                    command_list,
                    render_pass->_render_passes[_frame_index], 
                    render_pass->_color_clears, 
                    render_pass->_depth_stencil_clear.first,
                    render_pass->_depth_stencil_clear.second
                );

                RenderPassContext context;
                context._render_pass = render_pass->_render_passes[_frame_index];
                context._command_list = command_list;
                context._pass_index = pass_index;
                context._frame_index = _frame_index;
                context._attachments = std::span<Attachment const>(*render_pass->_input_attachments.data(), render_pass->_input_attachments.size());

                (*render_pass)(context);

                device.end_render_pass(command_list);

                ++pass_index;
            } break;
        }
    }
/*
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
*/
    _frame_index = (_frame_index + 1) % _frame_count;
    return 0;
}

void FrameGraph::bind_attachment(Attachment& attachment, backend::Handle<backend::Texture> const& texture) {

    assert(attachment.is_persistent() && "attachment is not persistent");

    if(attachment._attachments[_frame_index] != texture) {

        attachment._attachments[_frame_index] = texture;

        for(auto& render_pass : attachment._render_passes) {
            render_pass->_is_compiled = false;
        }
    }
}

void FrameGraph::compile_render_pass(backend::Device& device, RenderPass& render_pass, uint32_t const frame_index) {

    std::array<backend::Handle<backend::Texture>, 8> colors;
    std::array<backend::RenderPassColorDesc, 8> color_descs;
    backend::Handle<backend::Texture> depth_stencil;

    for(size_t i = 0; i < render_pass._color_attachments.size(); ++i) {
        colors[i] = render_pass._color_attachments[i]->_attachments[frame_index];

        auto color_desc = backend::RenderPassColorDesc {};
        color_desc.load_op = render_pass._color_ops[i];
        color_desc.store_op = backend::RenderPassStoreOp::Store;

        color_descs[i] = color_desc;
    }

    if(!render_pass._depth_stencil_attachment) {

        depth_stencil = render_pass._depth_stencil_attachment->_attachments[frame_index];

        auto depth_stencil_desc = backend::RenderPassDepthStencilDesc {};
        depth_stencil_desc.depth_load_op = render_pass._depth_stencil_op;
        depth_stencil_desc.depth_store_op = backend::RenderPassStoreOp::Store;
        depth_stencil_desc.stencil_load_op = render_pass._depth_stencil_op;
        depth_stencil_desc.stencil_store_op = backend::RenderPassStoreOp::Store;

        device.create_render_pass(
            std::span<backend::Handle<backend::Texture>>(colors.data(), render_pass._color_attachments.size()),
            std::span<backend::RenderPassColorDesc>(color_descs.data(), render_pass._color_attachments.size()),
            depth_stencil,
            depth_stencil_desc
        );
    } else {

        device.create_render_pass(
            std::span<backend::Handle<backend::Texture>>(colors.data(), render_pass._color_attachments.size()),
            std::span<backend::RenderPassColorDesc>(color_descs.data(), render_pass._color_attachments.size()),
            backend::InvalidHandle<backend::Texture>(),
            {}
        );
    }
}
