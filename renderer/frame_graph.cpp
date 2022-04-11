// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine::renderer;

Attachment& FrameGraph::add_attachment(AttachmentDesc const& attachment_desc) {

    _attachments.push_back(std::make_unique<Attachment>(attachment_desc));
    return *_attachments.back().get();
}

Attachment& FrameGraph::add_attachment(std::string_view const name, backend::MemoryState const before, backend::MemoryState const after) {

    _attachments.push_back(std::make_unique<Attachment>(name, before, after));
    return *_attachments.back().get();
}

RenderPass& FrameGraph::add_pass(RenderPassDesc const& render_pass_desc, RenderPassFunc const& func) {

    _ops.emplace_back(OpType::RenderPass, static_cast<uint32_t>(_render_passes.size()));
    _render_passes.push_back(std::make_unique<RenderPass>(render_pass_desc, func));
    return *_render_passes.back().get();
}

ComputePass& FrameGraph::add_pass(ComputePassDesc const& compute_pass_desc, ComputePassFunc const& func) {

    assert(false && "to do add compute pass");

    _ops.emplace_back(OpType::ComputePass, static_cast<uint32_t>(_compute_passes.size()));
    _compute_passes.push_back(std::make_unique<ComputePass>(compute_pass_desc, func));
    return *_compute_passes.back().get();
}

void FrameGraph::build(backend::Device& device, uint32_t const frame_count) {

    _frame_count = frame_count;

    std::vector<OpData> temp_ops;

    for(size_t i = 0; i < _attachments.size(); ++i) {
        _attachments[i]->_attachments.resize(frame_count);

        for(uint32_t j = 0; j < frame_count; ++j) {

            if(_attachments[i]->is_persistent()) {
                _attachment_barriers[_attachments[i]->hash()].state = _attachments[i]->_before;
                _attachments[i]->_attachments[j] = backend::InvalidHandle<backend::Texture>();
            } else {
                _attachment_barriers[_attachments[i]->hash()].state = backend::MemoryState::Common;
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

    std::vector<backend::Handle<backend::CommandList>> cur_graphics_command_lists;
    cur_graphics_command_lists.resize(frame_count);

    std::vector<backend::Handle<backend::CommandList>> cur_compute_command_lists;
    cur_compute_command_lists.resize(frame_count);

    for(auto& op : _ops) {

        switch(op.op_type) {

            case OpType::RenderPass: {

                auto& render_pass = _render_passes[op.index];

                bool is_external_render_pass = false;

                for(auto& attachment : render_pass->_color_attachments) {
                    if(attachment->is_persistent()) {
                        is_external_render_pass = true;
                    }

                    attachment->_render_passes.emplace_back(render_pass.get());

                    if(_attachment_barriers[attachment->hash()].state != backend::MemoryState::RenderTarget) {
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::RenderTarget;
                        _attachment_barriers[attachment->hash()].switch_index.emplace(render_pass->hash());
                    }
                }

                if(render_pass->_depth_stencil_attachment) {
                    if(render_pass->_depth_stencil_attachment->is_persistent()) {
                        is_external_render_pass = true;
                    }

                    render_pass->_depth_stencil_attachment->_render_passes.emplace_back(render_pass.get());

                    if(_attachment_barriers[render_pass->_depth_stencil_attachment->hash()].state != backend::MemoryState::DepthWrite) {
                        _attachment_barriers[render_pass->_depth_stencil_attachment->hash()].state = backend::MemoryState::DepthWrite;
                        _attachment_barriers[render_pass->_depth_stencil_attachment->hash()].switch_index.emplace(render_pass->hash());
                    }
                }

                for(auto& attachment : render_pass->_input_attachments) {
                    if(attachment->is_persistent()) {
                        is_external_render_pass = true;
                    }

                    attachment->_render_passes.emplace_back(render_pass.get());

                    if(_attachment_barriers[attachment->hash()].state != backend::MemoryState::ShaderRead) {
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::ShaderRead;
                        _attachment_barriers[attachment->hash()].switch_index.emplace(render_pass->hash());
                    }
                }

                for(uint32_t i = 0; i < frame_count; ++i) {

                    render_pass->_render_passes.resize(frame_count);
                    render_pass->_command_lists.resize(frame_count);

                    if(!is_external_render_pass) {
                        compile_render_pass(device, *render_pass, i);
                    }

                    if(cur_graphics_command_lists[i] == backend::InvalidHandle<backend::CommandList>()) {
                        cur_graphics_command_lists[i] = _command_lists.emplace_back(device.create_command_list(backend::QueueFlags::Graphics));
                    }

                    render_pass->_command_lists[i] = cur_graphics_command_lists[i];
                }

                temp_ops.emplace_back(OpType::RenderPass, op.index);
            } break;

            case OpType::ComputePass: {
                // TODO!
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

    {
        temp_ops.emplace_back(OpType::SubmitCommandList, static_cast<uint32_t>(_submit_command_lists.size()));

        _submit_command_lists.emplace_back(
            cur_graphics_command_lists,
            backend::QueueFlags::Graphics
        );
    }

    _ops = std::move(temp_ops);
}

void FrameGraph::reset(backend::Device& device) {

    device.wait_for_idle(backend::QueueFlags::Graphics | backend::QueueFlags::Compute);

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

uint64_t FrameGraph::execute(backend::Device& device) {

    uint32_t pass_index = 0;
    uint64_t fence_value = 0;

    for(auto& op : _ops) {
        switch(op.op_type) {
            case OpType::RenderPass: {

                auto& render_pass = _render_passes[op.index];

                if(!render_pass->is_compiled()) {

                    if(render_pass->_render_passes[_frame_index] != backend::InvalidHandle<backend::RenderPass>()) {
                        device.delete_render_pass(render_pass->_render_passes[_frame_index]);
                    }

                    compile_render_pass(device, *render_pass, _frame_index);
                }

                device.set_viewport(render_pass->_command_lists[_frame_index], 0, 0, render_pass->_width, render_pass->_height);
                device.set_scissor(render_pass->_command_lists[_frame_index], 0, 0, render_pass->_width, render_pass->_height);

                // Color Barriers
                for(auto& attachment : render_pass->_color_attachments) {

                    if(_attachment_barriers[attachment->hash()].switch_index.find(render_pass->hash()) != _attachment_barriers[attachment->hash()].switch_index.end()) {
                        device.barrier(render_pass->_command_lists[_frame_index], attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, backend::MemoryState::RenderTarget);
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::RenderTarget;
                    }
                }

                // Depth Stencil Barrier
                if(render_pass->_depth_stencil_attachment)
                {
                    auto& attachment = render_pass->_depth_stencil_attachment;

                    if(_attachment_barriers[attachment->hash()].switch_index.find(render_pass->hash()) != _attachment_barriers[attachment->hash()].switch_index.end()) {
                        device.barrier(render_pass->_command_lists[_frame_index], attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, backend::MemoryState::DepthWrite);
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::DepthWrite;
                    }
                }

                // Input Barriers
                for(auto& attachment : render_pass->_input_attachments) {

                    if(_attachment_barriers[attachment->hash()].switch_index.find(render_pass->hash()) != _attachment_barriers[attachment->hash()].switch_index.end()) {
                        device.barrier(render_pass->_command_lists[_frame_index], attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, backend::MemoryState::ShaderRead);
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::ShaderRead;
                    }
                }

                device.begin_render_pass(
                    render_pass->_command_lists[_frame_index],
                    render_pass->_render_passes[_frame_index], 
                    render_pass->_color_clears, 
                    render_pass->_depth_stencil_clear.first,
                    render_pass->_depth_stencil_clear.second
                );

                RenderPassContext context;
                context._render_pass = render_pass->_render_passes[_frame_index];
                context._command_list = render_pass->_command_lists[_frame_index];
                context._pass_index = pass_index;
                context._frame_index = _frame_index;
                context._attachments = std::span<Attachment const* const>(render_pass->_input_attachments.data(), render_pass->_input_attachments.size());

                (*render_pass)(context);

                device.end_render_pass(render_pass->_command_lists[_frame_index]);

                ++pass_index;
            } break;

            case OpType::SubmitCommandList: {

                auto& submit_command_list = _submit_command_lists[op.index];

                for(auto& attachment : _attachments) {

                    if(attachment->is_persistent()) {
                        device.barrier(submit_command_list.command_lists[_frame_index], attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, attachment->_after);
                        _attachment_barriers[attachment->hash()].state = attachment->_after;
                    } else {
                        device.barrier(submit_command_list.command_lists[_frame_index], attachment->_attachments[_frame_index], _attachment_barriers[attachment->hash()].state, backend::MemoryState::Common);
                        _attachment_barriers[attachment->hash()].state = backend::MemoryState::Common;
                    }
                }

                fence_value = device.submit(std::span<backend::Handle<backend::CommandList> const>(&submit_command_list.command_lists[_frame_index], 1), submit_command_list.flags);
            } break;
        }
    }

    _frame_index = (_frame_index + 1) % _frame_count;
    return fence_value;
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

    if(render_pass._depth_stencil_attachment) {

        depth_stencil = render_pass._depth_stencil_attachment->_attachments[frame_index];

        auto depth_stencil_desc = backend::RenderPassDepthStencilDesc {};
        depth_stencil_desc.depth_load_op = render_pass._depth_stencil_op;
        depth_stencil_desc.depth_store_op = backend::RenderPassStoreOp::Store;
        depth_stencil_desc.stencil_load_op = render_pass._depth_stencil_op;
        depth_stencil_desc.stencil_store_op = backend::RenderPassStoreOp::Store;

        render_pass._render_passes[frame_index] = device.create_render_pass(
            std::span<backend::Handle<backend::Texture>>(colors.data(), render_pass._color_attachments.size()),
            std::span<backend::RenderPassColorDesc>(color_descs.data(), render_pass._color_attachments.size()),
            depth_stencil,
            depth_stencil_desc
        );
    } else {

        render_pass._render_passes[frame_index] = device.create_render_pass(
            std::span<backend::Handle<backend::Texture>>(colors.data(), render_pass._color_attachments.size()),
            std::span<backend::RenderPassColorDesc>(color_descs.data(), render_pass._color_attachments.size()),
            backend::InvalidHandle<backend::Texture>(),
            {}
        );
    }

    render_pass._is_compiled = true;
}
