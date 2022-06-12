// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>
#include <lib/thread_pool.h>

using namespace ionengine;
using namespace ionengine::renderer;

FrameGraph::FrameGraph(backend::Device& device) :
    _device(&device) {

    uint32_t const FRAME_GRAPH_RENDER_PASS_MT_COUNT = 4;
    uint32_t const FRAME_GRAPH_RENDER_PASS_ST_COUNT = 10;
    uint32_t const FRAME_GRAPH_GRAPHICS_BUNDLE_COUNT = FRAME_GRAPH_RENDER_PASS_MT_COUNT * lib::thread_pool().size() + FRAME_GRAPH_RENDER_PASS_ST_COUNT;
    uint32_t const FRAME_GRAPH_GRAPHICS_COMMAND_COUNT = FRAME_GRAPH_RENDER_PASS_MT_COUNT + FRAME_GRAPH_RENDER_PASS_ST_COUNT;
    uint32_t const FRAME_GRAPH_COMPUTE_COMMAND_COUNT = 1;

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _graphics_command_pools.emplace_back(*_device, FRAME_GRAPH_GRAPHICS_COMMAND_COUNT);
        _compute_command_pools.emplace_back(*_device, FRAME_GRAPH_COMPUTE_COMMAND_COUNT);
        _graphics_bundle_command_pools.emplace_back(*_device, FRAME_GRAPH_GRAPHICS_BUNDLE_COUNT);
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
    RenderPassFunc const& func,
    uint64_t& cache
) {
    auto it = _render_pass_cache.find(cache);

    if(it != _render_pass_cache.end()) {

        _ops.emplace_back(OpType::RenderPass, _render_passes.size());
        _render_passes.emplace_back(it->second);

    } else {

        auto result = RenderPass::create(*_device, name, width, height, colors, inputs, depth_stencil, func, _swapchain);

        if(result.is_ok()) {

            RenderPass render_pass = std::move(result.as_ok());
            cache = render_pass.hash;

            ResourcePtr<RenderPass> ptr = make_resource_ptr(std::move(render_pass));

            _render_pass_cache.insert({ cache, ptr });

            _ops.emplace_back(OpType::RenderPass, _render_passes.size());
            _render_passes.emplace_back(std::move(ptr));

        } else {
            throw lib::Exception(result.as_error().message);
        }
    }
}

void FrameGraph::reset() {
    _device->wait_for_idle(backend::QueueFlags::Graphics | backend::QueueFlags::Compute);
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
                    .command_pool = &_graphics_bundle_command_pools.at(_frame_index)
                };
                PassTaskCreateInfo pass_task_create_info = render_pass->get().func(context);

                ResourcePtr<CommandList> command_list = _graphics_command_pools.at(_frame_index).allocate();

                _device->set_viewport(command_list->get().command_list, 0, 0, render_pass->get().width, render_pass->get().height);
                _device->set_scissor(command_list->get().command_list, 0, 0, render_pass->get().width, render_pass->get().height);

                std::vector<backend::MemoryBarrierDesc> memory_barriers;
                bool is_swapchain_used = false;
        
                for(auto& attachment : render_pass->get().color_attachments) {
                    if(!attachment) {
                        if(_swapchain.memory_state != backend::MemoryState::RenderTarget) {
                            memory_barriers.emplace_back(_swapchain.texture, backend::MemoryState::Present, backend::MemoryState::RenderTarget);
                            _swapchain.memory_state = backend::MemoryState::RenderTarget;
                            is_swapchain_used = true;
                        }
                    } else {
                        if(attachment->get().memory_state != backend::MemoryState::RenderTarget) {
                            memory_barriers.emplace_back(attachment->get().barrier(backend::MemoryState::RenderTarget));
                        }
                    }
                }

                if(auto& attachment = render_pass->get().ds_attachment) {
                    if(attachment->get().memory_state != backend::MemoryState::DepthWrite) {
                        memory_barriers.emplace_back(attachment->get().barrier(backend::MemoryState::DepthWrite));
                    }
                }

                for(auto& attachment : render_pass->get().inputs) {
                    if(attachment->get().memory_state != backend::MemoryState::ShaderRead && attachment->get().is_render_target()) {
                        memory_barriers.emplace_back(attachment->get().barrier(backend::MemoryState::ShaderRead));
                    }
                    if(attachment->get().memory_state != backend::MemoryState::DepthRead && attachment->get().is_depth_stencil()) {
                        memory_barriers.emplace_back(attachment->get().barrier(backend::MemoryState::DepthRead));
                    }
                }

                if(!memory_barriers.empty()) {
                    _device->barrier(command_list->get().command_list, memory_barriers);
                }

                _device->begin_render_pass(
                    command_list->get().command_list,
                    render_pass->get().render_pass,
                    render_pass->get().color_clears,
                    render_pass->get().ds_clear.first,
                    render_pass->get().ds_clear.second
                );

                auto pass_task_visitor = make_visitor(
                    [&](PassTaskData<PassTaskType::Single> const& data) {
                        _device->execute_bundle(command_list->get().command_list, data.command_list->get().command_list);
                    },
                    [&](PassTaskData<PassTaskType::Multithreading> const& data) {
                        lib::thread_pool().wait_all(lib::TaskPriorityFlags::High);

                        for(auto const& bundle_command_list : data.command_lists) {
                            _device->execute_bundle(command_list->get().command_list, bundle_command_list->get().command_list);
                        }
                    }
                );
                std::visit(pass_task_visitor, pass_task_create_info.data);
                
                _device->end_render_pass(command_list->get().command_list);

                if(is_swapchain_used) {
                    if(_swapchain.memory_state != backend::MemoryState::Common) {
                        
                        auto memory_barrier = backend::MemoryBarrierDesc {
                            .target = _swapchain.texture,
                            .before = backend::MemoryState::RenderTarget,
                            .after = backend::MemoryState::Common
                        };

                        _device->barrier(command_list->get().command_list, std::span<backend::MemoryBarrierDesc const>(&memory_barrier, 1));
                        _swapchain.memory_state = backend::MemoryState::Common;
                    }
                }

                _fence_values.at(_frame_index) = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&command_list->get().command_list, 1), backend::QueueFlags::Graphics);
            } break;
        }
    }

    _device->present();

    _frame_index = (_frame_index + 1) % backend::BACKEND_BACK_BUFFER_COUNT;

    _render_passes.clear();
    _ops.clear();
}

uint32_t FrameGraph::wait() {
    _device->wait(_fence_values.at(_frame_index), backend::QueueFlags::Graphics);
    _swapchain.texture = _device->acquire_next_texture();
    return _frame_index;
}
