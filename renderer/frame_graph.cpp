// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine;
using namespace ionengine::renderer;

FrameGraph::FrameGraph(backend::Device& device) :
    _device(&device) {

    _command_lists.resize(_frame_count);
    for(uint32_t i = 0; i < _frame_count; ++i) {
        _command_lists.at(i) = _device->create_command_list(backend::QueueFlags::Graphics);
    }
    _fence_values.resize(_frame_count);
    _swapchain_texture = _device->acquire_next_texture();
    _swapchain_memory_state = backend::MemoryState::Present;
}

void FrameGraph::add_pass(
    std::string_view const name, 
    uint32_t const width,
    uint32_t const height,
    std::optional<std::span<CreateColorInfo const>> const colors,
    std::optional<std::span<CreateInputInfo const>> const inputs, 
    std::optional<CreateDepthStencilInfo> const depth_stencil,
    RenderPassFunc const& func
) {

    std::string render_pass_name = std::format("{}_{}", name, _frame_index);
    uint32_t const render_pass_hash = lib::hash::ctcrc32(render_pass_name.data(), render_pass_name.size());

    if(_render_pass_cache.find(render_pass_hash) == _render_pass_cache.end()) {
        std::vector<std::shared_ptr<GPUTexture>> color_attachments;
        std::vector<backend::RenderPassLoadOp> color_ops;
        std::vector<lib::math::Color> color_clears;
        std::vector<std::shared_ptr<GPUTexture>> inputs;

        for(auto const& color : colors.value()) {
            color_attachments.emplace_back(color.attachment);
            color_ops.emplace_back(color.load_op);
            color_clears.emplace_back(color.clear_color);
        }



        auto depth_stencil_info = depth_stencil.value_or(
            CreateDepthStencilInfo { 
                .attachment = nullptr, 
                .load_op = backend::RenderPassLoadOp::DontCare,
                .clear_depth = 0.0f, 
                .clear_stencil = 0x0
            }
        );

        auto render_pass = RenderPass {
            .name = std::string(name),
            .width = width,
            .height = height,
            .color_attachments = color_attachments,
            .color_ops = color_ops,
            .color_clears = color_clears,
            .ds_attachment = depth_stencil_info.attachment,
            .ds_op = depth_stencil_info.load_op,
            .ds_clear = { depth_stencil_info.clear_depth, depth_stencil_info.clear_stencil },
            .inputs = inputs,
            .func = func,
            .render_pass = compile_render_pass(color_attachments, color_ops, depth_stencil_info.attachment, depth_stencil_info.load_op),
            .command_list = _command_lists.at(_frame_index)
        };

        _render_pass_cache.insert({ render_pass_hash, std::move(render_pass) });
    }

    _ops.emplace_back(OpType::RenderPass, _render_passes.size());
    _render_passes.emplace_back(&_render_pass_cache.at(render_pass_hash));
}

void FrameGraph::reset() {
    _device->wait_for_idle(backend::QueueFlags::Graphics | backend::QueueFlags::Compute);
}

void FrameGraph::execute() {

    for(auto const& op : _ops) {

        switch(op.op_type) {
            case OpType::RenderPass: {
                auto& render_pass = _render_passes[op.index];

                _device->set_viewport(render_pass->command_list, 0, 0, render_pass->width, render_pass->height);
                _device->set_scissor(render_pass->command_list, 0, 0, render_pass->width, render_pass->height);

                for(auto& attachment : render_pass->color_attachments) {
                    if(!attachment) {
                        if(_swapchain_memory_state != backend::MemoryState::RenderTarget) {
                            _device->barrier(render_pass->command_list, _swapchain_texture, backend::MemoryState::Present, backend::MemoryState::RenderTarget);
                            _swapchain_memory_state = backend::MemoryState::RenderTarget;
                        }
                    } else {
                        if(attachment->memory_state() != backend::MemoryState::RenderTarget) {
                            attachment->barrier(render_pass->command_list, backend::MemoryState::RenderTarget);
                        }
                    }
                }

                if(auto& attachment = render_pass->ds_attachment) {
                    if(attachment->memory_state() != backend::MemoryState::DepthWrite) {
                        attachment->barrier(render_pass->command_list, backend::MemoryState::DepthWrite);
                    }
                }

                for(auto& attachment : render_pass->inputs) {
                    if(attachment->memory_state() != backend::MemoryState::ShaderRead && attachment->is_render_target()) {
                        attachment->barrier(render_pass->command_list, backend::MemoryState::ShaderRead);
                    }
                    if(attachment->memory_state() != backend::MemoryState::DepthRead && attachment->is_depth_stencil()) {
                        attachment->barrier(render_pass->command_list, backend::MemoryState::DepthRead);
                    }
                }

                _device->begin_render_pass(
                    render_pass->command_list,
                    render_pass->render_pass,
                    render_pass->color_clears,
                    render_pass->ds_clear.first,
                    render_pass->ds_clear.second
                );

                RenderPassContext context;
                context._inputs = render_pass->inputs;
                context._render_pass = render_pass->render_pass;
                context._command_list = render_pass->command_list;

                render_pass->func(context);

                _device->end_render_pass(render_pass->command_list);

                if(_swapchain_memory_state != backend::MemoryState::Present) {
                    _device->barrier(render_pass->command_list, _swapchain_texture, _swapchain_memory_state, backend::MemoryState::Present);
                    _swapchain_memory_state = backend::MemoryState::Present;
                }
            } break;
        }
    }

    _fence_values.at(_frame_index) = _device->submit(
        std::span<backend::Handle<backend::CommandList> const>(&_command_lists.at(_frame_index), 1), 
        backend::QueueFlags::Graphics
    );

    _device->present();

    _frame_index = (_frame_index + 1) % _frame_count;

    _render_passes.clear();
    _ops.clear();
}

uint32_t FrameGraph::wait() {
    _device->wait(_fence_values.at(_frame_index), backend::QueueFlags::Graphics);
    _swapchain_texture = _device->acquire_next_texture();
    return _frame_index;
}

backend::Handle<backend::RenderPass> FrameGraph::compile_render_pass(
    std::span<std::shared_ptr<GPUTexture>> color_attachments,
    std::span<backend::RenderPassLoadOp> color_ops,
    std::shared_ptr<GPUTexture> depth_stencil_attachment,
    backend::RenderPassLoadOp depth_stencil_op
) {

    std::vector<backend::Handle<backend::Texture>> colors;
    std::vector<backend::RenderPassColorDesc> color_descs;

    assert(color_attachments.size() == color_ops.size() && "color attachments and color ops has different sizes");
    size_t const color_attachments_size = color_attachments.size();

    colors.resize(color_attachments_size);
    color_descs.resize(color_attachments_size);

    for(size_t i = 0; i < color_attachments_size; ++i) {
        if(!color_attachments[i]) {
            colors[i] = _swapchain_texture;
        } else {
            colors[i] = color_attachments[i]->as_texture();
        }
        color_descs[i] = backend::RenderPassColorDesc {
            .load_op = color_ops[i],
            .store_op = backend::RenderPassStoreOp::Store
        };
    }

    if(depth_stencil_attachment) {
        backend::Handle<backend::Texture> depth_stencil = depth_stencil_attachment->as_texture();

        auto depth_stencil_desc = backend::RenderPassDepthStencilDesc {
            .depth_load_op = depth_stencil_op,
            .depth_store_op = backend::RenderPassStoreOp::Store,
            .stencil_load_op = depth_stencil_op,
            .stencil_store_op = backend::RenderPassStoreOp::Store
        };

        return _device->create_render_pass(colors, color_descs, depth_stencil, depth_stencil_desc);
    } else {
        return _device->create_render_pass(colors, color_descs, backend::InvalidHandle<backend::Texture>(), {});
    }
}
