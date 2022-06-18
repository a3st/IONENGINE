// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>
#include <renderer/gpu_texture.h>
#include <lib/thread_pool.h>
#include <range/v3/view.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

FrameGraph::FrameGraph(backend::Device& device) :
    _device(&device),
    _render_pass_cache(device) {

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
}

void FrameGraph::add_pass(
    std::string_view const name, 
    uint32_t const width,
    uint32_t const height,
    std::span<CreateColorInfo const> const colors,
    std::span<CreateInputInfo const> const inputs,
    std::optional<CreateDepthStencilInfo> const depth_stencil,
    RenderPassFunc const& func
) {

    std::vector<backend::RenderPassColorDesc> color_descs;

    for(auto const& color : colors) {
        color_descs.push_back(
            backend::RenderPassColorDesc {
                .load_op = color.load_op,
                .store_op = backend::RenderPassStoreOp::Store
            }
        );
    }

    auto color_textures = 
        colors | 
        std::views::transform([&](CreateColorInfo const& info) { return info.attachment; }) | 
        ranges::to<std::vector<ResourcePtr<GPUTexture>>>();

    auto color_texture_ptrs = 
        colors | 
        std::views::transform([&](CreateColorInfo const& info) { return &info.attachment->get(); }) | 
        ranges::to<std::vector<GPUTexture const*>>();

    auto color_clears =
        colors |
        std::views::transform([&](CreateColorInfo const& info) { return info.clear_color; }) | 
        ranges::to<std::vector<lib::math::Color>>();

    auto input_textures =
        inputs |
        std::views::transform([&](CreateInputInfo const& info) { return info.attachment; }) | 
        ranges::to<std::vector<ResourcePtr<GPUTexture>>>();

    ResourcePtr<RenderPass> render_pass;

    if(depth_stencil.has_value()) {
        render_pass = _render_pass_cache.get(
            color_texture_ptrs, 
            color_descs, 
            &depth_stencil.value().attachment->get(),
            backend::RenderPassDepthStencilDesc {
                .depth_load_op = depth_stencil.value().load_op,
                .depth_store_op = backend::RenderPassStoreOp::Store,
                .stencil_load_op = depth_stencil.value().load_op,
                .stencil_store_op = backend::RenderPassStoreOp::Store
            }
        );
    } else {
        render_pass = _render_pass_cache.get(
            color_texture_ptrs, 
            color_descs, 
            nullptr,
            std::nullopt
        );
    }

    PassTaskData<PassTaskType::RenderPass> render_pass_task;

    if(depth_stencil.has_value()) {
        render_pass_task = PassTaskData<PassTaskType::RenderPass> {
            .width = width,
            .height = height,
            .color_clears = std::move(color_clears),
            .colors = std::move(color_textures),
            .inputs = std::move(input_textures),
            .depth_stencil = depth_stencil.value().attachment,
            .depth_clear = depth_stencil.value().clear_depth,
            .stencil_clear = depth_stencil.value().clear_stencil,
            .func = func,
            .render_pass = std::move(render_pass)
        };
    } else {
        render_pass_task = PassTaskData<PassTaskType::RenderPass> {
            .width = width,
            .height = height,
            .color_clears = std::move(color_clears),
            .colors = std::move(color_textures),
            .inputs = std::move(input_textures),
            .depth_stencil = nullptr,
            .depth_clear = 0.0f,
            .stencil_clear = 0x0,
            .func = func,
            .render_pass = std::move(render_pass)
        };
    }

    _tasks.push_back(
        PassTask {
            .name = std::string(name),
            .data = std::move(render_pass_task)
        }
    );
}

void FrameGraph::add_present_pass(ResourcePtr<GPUTexture> swap_color) {

    auto present_pass_task = PassTaskData<PassTaskType::PresentPass> {
        .swap_color = swap_color
    };

    _tasks.push_back(
        PassTask {
            .name = "present",
            .data = std::move(present_pass_task)
        }
    );
}

void FrameGraph::reset() {
    _device->wait_for_idle(backend::QueueFlags::Graphics | backend::QueueFlags::Compute);
}

void FrameGraph::execute() {
    _graphics_command_pools.at(_frame_index).reset();
    _compute_command_pools.at(_frame_index).reset();
    _graphics_bundle_command_pools.at(_frame_index).reset();

    for(auto& task : _tasks) {
        auto task_visitor = make_visitor(
            [&](PassTaskData<PassTaskType::RenderPass>& data) {
                ResourcePtr<CommandList> command_list = _graphics_command_pools.at(_frame_index).allocate();

                auto context = RenderPassContext {
                    .render_pass = data.render_pass,
                    .command_pool = &_graphics_bundle_command_pools.at(_frame_index)
                };
                PassTaskResult task_result = data.func(context);

                _device->set_viewport(command_list->get().command_list, 0, 0, data.width, data.height);
                _device->set_scissor(command_list->get().command_list, 0, 0, data.width, data.height);

                std::vector<backend::MemoryBarrierDesc> memory_barriers;
        
                for(auto& color : data.colors) {
                    if(color->get().memory_state != backend::MemoryState::RenderTarget) {
                        memory_barriers.emplace_back(color->get().barrier(backend::MemoryState::RenderTarget));
                    }
                }

                if(auto& depth_stencil = data.depth_stencil) {
                    if(depth_stencil->get().memory_state != backend::MemoryState::DepthWrite) {
                        memory_barriers.emplace_back(depth_stencil->get().barrier(backend::MemoryState::DepthWrite));
                    }
                }

                for(auto& input : data.inputs) {
                    if(input->get().memory_state != backend::MemoryState::ShaderRead && input->get().is_render_target()) {
                        memory_barriers.emplace_back(input->get().barrier(backend::MemoryState::ShaderRead));
                    } else if(input->get().memory_state != backend::MemoryState::DepthRead && input->get().is_depth_stencil()) {
                        memory_barriers.emplace_back(input->get().barrier(backend::MemoryState::DepthRead));
                    }
                }

                if(!memory_barriers.empty()) {
                    _device->barrier(command_list->get().command_list, memory_barriers);
                }

                data.render_pass->get().begin(
                    *_device,
                    command_list->get(),
                    data.color_clears,
                    data.depth_clear,
                    data.stencil_clear
                );

                auto task_result_visitor = make_visitor(
                    [this, &command_list](PassTaskResultData<PassTaskResultType::Single> const& data) {
                        _device->execute_bundle(command_list->get().command_list, data.command_list->get().command_list);
                    },
                    [this, &command_list](PassTaskResultData<PassTaskResultType::Multithreading> const& data) {
                        lib::thread_pool().wait_all(lib::TaskPriorityFlags::High);
                        for(auto const& bundle_command_list : data.command_lists) {
                            _device->execute_bundle(command_list->get().command_list, bundle_command_list->get().command_list);
                        }
                    }
                );
                std::visit(task_result_visitor, task_result.data);
                
                data.render_pass->get().end(*_device, command_list->get());

                _fence_values.at(_frame_index) = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&command_list->get().command_list, 1), backend::QueueFlags::Graphics);
            },
            [&](PassTaskData<PassTaskType::ComputePass>& data) { },
            [&](PassTaskData<PassTaskType::PresentPass>& data) { 

                ResourcePtr<CommandList> command_list = _graphics_command_pools.at(_frame_index).allocate();

                std::vector<backend::MemoryBarrierDesc> memory_barriers;

                if(data.swap_color->get().memory_state != backend::MemoryState::Common) {
                    memory_barriers.push_back(data.swap_color->get().barrier(backend::MemoryState::Common));
                }

                if(!memory_barriers.empty()) {
                    _device->barrier(command_list->get().command_list, memory_barriers);
                }

                _fence_values.at(_frame_index) = _device->submit(std::span<backend::Handle<backend::CommandList> const>(&command_list->get().command_list, 1), backend::QueueFlags::Graphics);
            }
        );
        std::visit(task_visitor, task.data);
    }

    _device->present();
    _tasks.clear();
}

uint32_t FrameGraph::wait() {
    _device->wait(_fence_values.at(_frame_index), backend::QueueFlags::Graphics);
    _frame_index = _device->acquire_next_swapchain_texture();
    return _frame_index;
}
