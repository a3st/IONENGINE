// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>
#include <renderer/gpu_texture.h>
#include <lib/thread_pool.h>

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
    std::span<RenderPassColorInfo const> const color_infos,
    std::span<ResourcePtr<GPUTexture> const> const input_textures,
    std::optional<RenderPassDepthStencilInfo> const depth_stencil_info,
    RenderPassFunc const& func
) {
    ResourcePtr<RenderPass> render_pass;

    if(depth_stencil_info.has_value()) {
        auto color_textures = 
            color_infos | 
            std::views::transform([&](RenderPassColorInfo const& info) { return &info.texture->get(); }) |
            ranges::to<std::vector<GPUTexture const*>>();

        auto color_descs =
            color_infos |
            std::views::transform([&](RenderPassColorInfo const& info) { 
                return backend::RenderPassColorDesc {
                    .load_op = info.load_op,
                    .store_op = backend::RenderPassStoreOp::Store
                };
            }) | 
            ranges::to<std::vector<backend::RenderPassColorDesc>>();

        auto& depth_stencil_info_value = depth_stencil_info.value();

        render_pass = _render_pass_cache.get(
            color_textures, 
            color_descs, 
            &depth_stencil_info_value.texture->get(),
            backend::RenderPassDepthStencilDesc {
                .depth_load_op = depth_stencil_info_value.load_op,
                .depth_store_op = backend::RenderPassStoreOp::Store,
                .stencil_load_op = depth_stencil_info_value.load_op,
                .stencil_store_op = backend::RenderPassStoreOp::Store
            }
        );
    } else {
        auto color_textures = 
            color_infos | 
            std::views::transform([&](RenderPassColorInfo const& info) { return &info.texture->get(); }) | 
            ranges::to<std::vector<GPUTexture const*>>();

        auto color_descs =
            color_infos |
            std::views::transform([&](RenderPassColorInfo const& info) { 
                return backend::RenderPassColorDesc {
                    .load_op = info.load_op,
                    .store_op = backend::RenderPassStoreOp::Store
                };
            }) | 
            ranges::to<std::vector<backend::RenderPassColorDesc>>();

        render_pass = _render_pass_cache.get(
            color_textures, 
            color_descs, 
            nullptr,
            std::nullopt
        );
    }

    auto color_textures = 
        color_infos | 
        std::views::transform([&](RenderPassColorInfo const& info) { return info.texture; }) | 
        ranges::to<std::vector>();

    auto clear_colors =
        color_infos |
        std::views::transform([&](RenderPassColorInfo const& info) { return info.clear_color; }) | 
        ranges::to<std::vector>();

    PassTaskData<PassTaskType::RenderPass> render_pass_task_data;

    if(depth_stencil_info.has_value()) {
        auto& depth_stencil_value = depth_stencil_info.value();

        render_pass_task_data = PassTaskData<PassTaskType::RenderPass> {
            .width = width,
            .height = height,
            .clear_colors = std::move(clear_colors),
            .color_textures = std::move(color_textures),
            .input_textures = {},
            .depth_stencil_texture = depth_stencil_value.texture,
            .clear_depth = depth_stencil_value.clear_depth,
            .clear_stencil = depth_stencil_value.clear_stencil,
            .func = func,
            .render_pass = std::move(render_pass)
        };
    } else {
        render_pass_task_data = PassTaskData<PassTaskType::RenderPass> {
            .width = width,
            .height = height,
            .clear_colors = std::move(clear_colors),
            .color_textures = std::move(color_textures),
            .input_textures = {},
            .depth_stencil_texture = nullptr,
            .clear_depth = 0.0f,
            .clear_stencil = 0x0,
            .func = func,
            .render_pass = std::move(render_pass)
        };
    }

    _tasks.push_back(
        PassTask {
            .name = std::string(name),
            .data = std::move(render_pass_task_data)
        }
    );
}

void FrameGraph::add_present_pass(ResourcePtr<GPUTexture> swap_texture) {
    auto present_pass_task_data = PassTaskData<PassTaskType::PresentPass> {
        .swap_texture = swap_texture
    };

    _tasks.push_back(
        PassTask {
            .name = "present",
            .data = std::move(present_pass_task_data)
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

    std::vector<backend::MemoryBarrierDesc> memory_barriers;

    for(auto& task : _tasks) {

        memory_barriers.clear();

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
        
                for(auto& texture : data.color_textures) {
                    if(texture->get().memory_state != backend::MemoryState::RenderTarget) {
                        memory_barriers.emplace_back(texture->get().barrier(backend::MemoryState::RenderTarget));
                    }
                }

                if(auto& texture = data.depth_stencil_texture) {
                    if(texture->get().memory_state != backend::MemoryState::DepthWrite) {
                        memory_barriers.emplace_back(texture->get().barrier(backend::MemoryState::DepthWrite));
                    }
                }

                for(auto& texture : data.input_textures) {
                    if(texture->get().memory_state != backend::MemoryState::ShaderRead && texture->get().is_render_target()) {
                        memory_barriers.emplace_back(texture->get().barrier(backend::MemoryState::ShaderRead));
                    } else if(texture->get().memory_state != backend::MemoryState::DepthRead && texture->get().is_depth_stencil()) {
                        memory_barriers.emplace_back(texture->get().barrier(backend::MemoryState::DepthRead));
                    }
                }

                if(!memory_barriers.empty()) {
                    command_list->get().barrier(*_device, memory_barriers);
                }

                data.render_pass->get().begin(
                    *_device,
                    command_list->get(),
                    std::span<lib::math::Color const>(data.clear_colors.data(), data.clear_colors.size()),
                    data.clear_depth,
                    data.clear_stencil
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

                if(data.swap_texture->get().memory_state != backend::MemoryState::Common) {
                    memory_barriers.push_back(data.swap_texture->get().barrier(backend::MemoryState::Common));
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
