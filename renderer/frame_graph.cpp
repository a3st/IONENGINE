// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine;
using namespace ionengine::renderer;

FrameGraph::FrameGraph(lib::ThreadPool& thread_pool, backend::Device& device) :
    _device(&device), _thread_pool(&thread_pool) {

    uint16_t const thread_count = 7;

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _graphics_command_pools.emplace_back(*_device, thread_count * 16);
        _compute_command_pools.emplace_back(*_device, thread_count * 16);
    }

    _fence_values.resize(backend::BACKEND_BACK_BUFFER_COUNT);

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
    TaskExecution const task_execution,
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
            .task_execution = task_execution,
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

    auto make_present_barrier_command_list = [&](backend::Device& device, backend::Handle<backend::CommandList> const& command_list, RenderPass& render_pass) {
        std::vector<backend::MemoryBarrierDesc> barriers;

        if(_swapchain_memory_state != backend::MemoryState::Present) {
            barriers.emplace_back(_swapchain_texture, _swapchain_memory_state, backend::MemoryState::Present);
            _swapchain_memory_state = backend::MemoryState::Present;
        }
                
        if(!barriers.empty()) {
            device.barrier(command_list, barriers);
        }
    };

    auto make_barrier_command_list = [&](backend::Device& device, backend::Handle<backend::CommandList> const& command_list, RenderPass& render_pass) {
        std::vector<backend::MemoryBarrierDesc> barriers;
        
        for(auto& attachment : render_pass.color_attachments) {
            if(!attachment) {
                if(_swapchain_memory_state != backend::MemoryState::RenderTarget) {
                    barriers.emplace_back(_swapchain_texture, backend::MemoryState::Present, backend::MemoryState::RenderTarget);
                    _swapchain_memory_state = backend::MemoryState::RenderTarget;
                }
            } else {
                if(attachment->memory_state != backend::MemoryState::RenderTarget) {
                    barriers.emplace_back(attachment->barrier(backend::MemoryState::RenderTarget));
                }
            }
        }

        if(auto& attachment = render_pass.ds_attachment) {
            if(attachment->memory_state != backend::MemoryState::DepthWrite) {
                barriers.emplace_back(attachment->texture, attachment->memory_state, backend::MemoryState::DepthWrite);
                attachment->memory_state = backend::MemoryState::DepthWrite;
            }
        }

        for(auto& attachment : render_pass.inputs) {
            if(attachment->memory_state != backend::MemoryState::ShaderRead && attachment->is_render_target()) {
                barriers.emplace_back(attachment->barrier(backend::MemoryState::ShaderRead));
            }
            if(attachment->memory_state != backend::MemoryState::DepthRead && attachment->is_depth_stencil()) {
                barriers.emplace_back(attachment->barrier(backend::MemoryState::DepthRead));
            }
        }

        if(!barriers.empty()) {
            device.barrier(command_list, barriers);
        }
    };

    _graphics_command_pools.at(_frame_index).reset();
    _compute_command_pools.at(_frame_index).reset();

    for(auto const& op : _ops) {

        switch(op.op_type) {

            case OpType::RenderPass: {
                auto& render_pass = _render_passes[op.index];

                uint16_t const thread_count = 7;

                if(render_pass->task_execution == TaskExecution::Single) {

                    ResourcePtr<CommandList> command_list = _graphics_command_pools.at(_frame_index).allocate();

                    make_barrier_command_list(*_device, command_list->command_list, *render_pass);
                    worker_render_pass_command_list(*_device, command_list->command_list, 0, *render_pass);
                    make_present_barrier_command_list(*_device, command_list->command_list, *render_pass);

                    _fence_values.at(_frame_index) = _device->submit(std::span<backend::Handle<backend::CommandList>>(&command_list->command_list, 1), backend::QueueFlags::Graphics);

                } else {

                    std::vector<std::future<void>> jobs(thread_count);
                    std::vector<backend::Handle<backend::CommandList>> command_lists;

                    ResourcePtr<CommandList> command_list = _graphics_command_pools.at(_frame_index).allocate();

                    command_lists.emplace_back(command_list->command_list);

                    jobs.at(0) = _thread_pool->push(
                        [&, command_list](backend::Device& device, RenderPass& render_pass) {
                            //make_barrier_command_list(device, command_list->command_list, render_pass);
                            worker_render_pass_command_list(device, command_list->command_list, 0, render_pass);
                        },
                        std::ref(*_device),
                        std::ref(*render_pass)
                    );

                    for(uint16_t i = 1; i < thread_count; ++i) {

                        ResourcePtr<CommandList> _command_list = _graphics_command_pools.at(_frame_index).allocate();

                        command_lists.emplace_back(_command_list->command_list);

                        jobs.at(i) = _thread_pool->push(
                            [&, _command_list, i](backend::Device& device, RenderPass& render_pass) {
                                worker_render_pass_command_list(device, _command_list->command_list, i, render_pass);
                            },
                            std::ref(*_device),
                            std::ref(*render_pass)
                        );
                    }

                    for(auto const& job : jobs) {
                        job.wait();
                    }

                    _fence_values.at(_frame_index) = _device->submit(command_lists, backend::QueueFlags::Graphics);
                }
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
    _swapchain_texture = _device->acquire_next_texture();
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
            colors[i] = _swapchain_texture;
        } else {
            colors[i] = color_attachments[i]->texture;
        }
        color_descs[i] = backend::RenderPassColorDesc {
            .load_op = color_ops[i],
            .store_op = backend::RenderPassStoreOp::Store
        };
    }

    if(depth_stencil_attachment) {
        backend::Handle<backend::Texture> depth_stencil = depth_stencil_attachment->texture;

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

void ionengine::renderer::worker_render_pass_command_list(backend::Device& device, backend::Handle<backend::CommandList> const& command_list, uint16_t const thread_index, RenderPass& render_pass) {
    device.set_viewport(command_list, 0, 0, render_pass.width, render_pass.height);
    device.set_scissor(command_list, 0, 0, render_pass.width, render_pass.height);

    device.begin_render_pass(
        command_list,
        render_pass.render_pass,
        render_pass.color_clears,
        render_pass.ds_clear.first,
        render_pass.ds_clear.second
    );

    auto context = RenderPassContext {
        .thread_index = thread_index,
        .command_list = command_list,
        .render_pass = render_pass.render_pass
    };
    render_pass.func(context);

    device.end_render_pass(command_list);
}