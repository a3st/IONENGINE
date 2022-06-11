// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine;
using namespace ionengine::renderer;

FrameGraph::FrameGraph(backend::Device& device) :
    _device(&device) {

    uint16_t const thread_count = 7;

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _graphics_command_pools.emplace_back(*_device, 24);
        _compute_command_pools.emplace_back(*_device, 24);
        _graphics_bundle_command_pools.emplace_back(*_device, thread_count * 16);
    }

    _fence_values.resize(backend::BACKEND_BACK_BUFFER_COUNT);

    _swapchain = SwapchainTexture {
        .texture = _device->acquire_next_texture(),
        .memory_state = backend::MemoryState::Present
    };
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
        std::vector<ResourcePtr<GPUTexture>> color_attachments;
        std::vector<backend::RenderPassLoadOp> color_ops;
        std::vector<lib::math::Color> color_clears;
        std::vector<ResourcePtr<GPUTexture>> input_attachments;

        for(auto const& color : colors.value()) {
            color_attachments.emplace_back(color.attachment);
            color_ops.emplace_back(color.load_op);
            color_clears.emplace_back(color.clear_color);
        }

        if(inputs.has_value()) {
            for(auto const& input : inputs.value()) {
                input_attachments.emplace_back(input.attachment);
            }
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
            .inputs = input_attachments,
            .func = func,
            .render_pass = compile_render_pass(color_attachments, color_ops, depth_stencil_info.attachment, depth_stencil_info.load_op)
        };

        _render_pass_cache.insert({ render_pass_hash, std::move(render_pass) });
    }

    _ops.emplace_back(OpType::RenderPass, _render_passes.size());
    _render_passes.emplace_back(&_render_pass_cache.at(render_pass_hash));
}

void FrameGraph::add_pass(
    std::string_view const name, 
    std::optional<std::span<CreateStorageInfo const>> const storages,
    ComputePassFunc const& func
) {
    std::string compute_pass_name = std::format("{}_{}", name, _frame_index);
    uint32_t const compute_pass_hash = lib::hash::ctcrc32(compute_pass_name.data(), compute_pass_name.size());

    if(_compute_pass_cache.find(compute_pass_hash) == _compute_pass_cache.end()) {
        std::vector<ResourcePtr<GPUBuffer>> storage_buffers;

        if(storages.has_value()) {
            for(auto const& storage : storages.value()) {
                storage_buffers.emplace_back(storage.buffer);
            }
        }

        auto compute_pass = ComputePass {
            .name = std::string(name),
            .storages = storage_buffers,
            .func = func
        };

        _compute_pass_cache.insert({ compute_pass_hash, std::move(compute_pass) });
    }

    _ops.emplace_back(OpType::ComputePass, _compute_passes.size());
    _compute_passes.emplace_back(&_compute_pass_cache.at(compute_pass_hash));
}

void FrameGraph::reset() {
    _device->wait_for_idle(backend::QueueFlags::Graphics | backend::QueueFlags::Compute);

    _render_pass_cache.clear();
    _compute_pass_cache.clear();
    _render_passes.clear();
    _compute_passes.clear();
}

void FrameGraph::execute() {

    _graphics_command_pools.at(_frame_index).reset();
    _compute_command_pools.at(_frame_index).reset();
    _graphics_bundle_command_pools.at(_frame_index).reset();

    for(auto const& op : _ops) {

        switch(op.op_type) {

            case OpType::RenderPass: {
                auto& render_pass = _render_passes[op.index];

                auto context = RenderPassContext {
                    .render_pass = render_pass,
                    .command_pool = &_graphics_bundle_command_pools.at(_frame_index),
                    .device = _device,
                    .swapchain = &_swapchain
                };
                TaskCreateInfo create_info = render_pass->func(context);

                ResourcePtr<CommandList> command_list = _graphics_command_pools.at(_frame_index).allocate();

                _device->set_viewport(command_list->as_const_ok().command_list, 0, 0, render_pass->width, render_pass->height);
                _device->set_scissor(command_list->as_const_ok().command_list, 0, 0, render_pass->width, render_pass->height);

                std::vector<backend::MemoryBarrierDesc> memory_barriers;
                bool is_swapchain_used = false;
        
                for(auto& attachment : render_pass->color_attachments) {
                    if(!attachment) {
                        if(_swapchain.memory_state != backend::MemoryState::RenderTarget) {
                            memory_barriers.emplace_back(_swapchain.texture, backend::MemoryState::Present, backend::MemoryState::RenderTarget);
                            _swapchain.memory_state = backend::MemoryState::RenderTarget;
                            is_swapchain_used = true;
                        }
                    } else {
                        if(attachment->as_ok()->memory_state.load() != backend::MemoryState::RenderTarget) {
                            memory_barriers.emplace_back(attachment->as_ok()->barrier(backend::MemoryState::RenderTarget));
                        }
                    }
                }

                if(auto& attachment = render_pass->ds_attachment) {
                    if(attachment->as_ok()->memory_state.load() != backend::MemoryState::DepthWrite) {
                        memory_barriers.emplace_back(attachment->as_ok()->barrier(backend::MemoryState::DepthWrite));
                    }
                }

                for(auto& attachment : render_pass->inputs) {
                    if(attachment->as_ok()->memory_state.load() != backend::MemoryState::ShaderRead && attachment->as_const_ok().is_render_target()) {
                        memory_barriers.emplace_back(attachment->as_ok()->barrier(backend::MemoryState::ShaderRead));
                    }
                    if(attachment->as_ok()->memory_state.load() != backend::MemoryState::DepthRead && attachment->as_const_ok().is_depth_stencil()) {
                        memory_barriers.emplace_back(attachment->as_ok()->barrier(backend::MemoryState::DepthRead));
                    }
                }

                if(!memory_barriers.empty()) {
                    _device->barrier(command_list->as_const_ok().command_list, memory_barriers);
                }

                _device->begin_render_pass(
                    command_list->as_const_ok().command_list,
                    render_pass->render_pass,
                    render_pass->color_clears,
                    render_pass->ds_clear.first,
                    render_pass->ds_clear.second
                );

                _thread_pool->wait_all();

                for(auto const& bundle_command_list : create_info.command_lists) {
                    _device->execute_bundle(command_list->as_const_ok().command_list, bundle_command_list->as_const_ok().command_list);
                }

                _device->end_render_pass(command_list->as_const_ok().command_list);

                if(is_swapchain_used) {
                    if(_swapchain.memory_state != backend::MemoryState::Common) {
                        
                        auto memory_barrier = backend::MemoryBarrierDesc {
                            .target = _swapchain.texture,
                            .before = backend::MemoryState::RenderTarget,
                            .after = backend::MemoryState::Common
                        };

                        _device->barrier(command_list->as_const_ok().command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));
                        _swapchain.memory_state = backend::MemoryState::Common;
                    }
                }

                _fence_values.at(_frame_index) = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&command_list->as_const_ok().command_list, 1), backend::QueueFlags::Graphics);
            } break;
        }
    }

    _device->present();

    _frame_index = (_frame_index + 1) % backend::BACKEND_BACK_BUFFER_COUNT;

    _render_passes.clear();
    _compute_passes.clear();
    _ops.clear();
}

uint32_t FrameGraph::wait() {
    _device->wait(_fence_values.at(_frame_index), backend::QueueFlags::Graphics);
    _swapchain.texture = _device->acquire_next_texture();
    return _frame_index;
}

backend::Handle<backend::RenderPass> FrameGraph::compile_render_pass(
    std::span<ResourcePtr<GPUTexture>> color_attachments,
    std::span<backend::RenderPassLoadOp> color_ops,
    ResourcePtr<GPUTexture> depth_stencil_attachment,
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
            colors[i] = _swapchain.texture;
        } else {
            colors[i] = color_attachments[i]->as_const_ok().texture;
        }
        color_descs[i] = backend::RenderPassColorDesc {
            .load_op = color_ops[i],
            .store_op = backend::RenderPassStoreOp::Store
        };
    }

    if(depth_stencil_attachment) {
        backend::Handle<backend::Texture> depth_stencil = depth_stencil_attachment->as_const_ok().texture;

        auto depth_stencil_desc = backend::RenderPassDepthStencilDesc {
            .depth_load_op = depth_stencil_op,
            .depth_store_op = backend::RenderPassStoreOp::Store,
            .stencil_load_op = depth_stencil_op,
            .stencil_store_op = backend::RenderPassStoreOp::Store
        };

        return _device->create_render_pass(colors, color_descs, depth_stencil, depth_stencil_desc);
    } else {
        return _device->create_render_pass(colors, color_descs);
    }
}

std::vector<ResourcePtr<CommandList>> RenderPassContext::single_for(
    std::function<void(backend::Handle<backend::CommandList> const&, backend::Handle<backend::RenderPass> const&)> func
) {
    std::vector<ResourcePtr<CommandList>> command_lists(1);

    ResourcePtr<CommandList> command_list = command_pool->allocate();
    command_lists.at(0) = command_list;

    func(command_list->as_const_ok().command_list, render_pass->render_pass);
    device->close_command_list(command_list->as_const_ok().command_list);

    return std::move(command_lists);
}
