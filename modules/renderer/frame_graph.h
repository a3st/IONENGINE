// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/command_pool.h>
#include <renderer/render_pass_cache.h>

namespace ionengine::renderer {

enum class PassTaskType {
    RenderPass,
    ComputePass,
    PresentPass
};

enum class PassTaskResultType {
    Single,
    Multithreading
};

template<PassTaskResultType Type>
struct PassTaskResultData { };

template<>
struct PassTaskResultData<PassTaskResultType::Single> {  
    ResourcePtr<CommandList> command_list;
};

template<>
struct PassTaskResultData<PassTaskResultType::Multithreading> {  
    std::vector<ResourcePtr<CommandList>> command_lists;
};

struct PassTaskResult {
    std::variant<
        PassTaskResultData<PassTaskResultType::Single>,
        PassTaskResultData<PassTaskResultType::Multithreading>
    > data;

    static PassTaskResult singlethread(ResourcePtr<CommandList>&& command_list) {
        return PassTaskResult { .data = PassTaskResultData<PassTaskResultType::Single> { .command_list = std::move(command_list) } };
    }

    static PassTaskResult multithread(std::vector<ResourcePtr<CommandList>>&& command_lists) {
        return PassTaskResult { .data = PassTaskResultData<PassTaskResultType::Multithreading> { .command_lists = std::move(command_lists) } };
    }
};

struct RenderPassContext {
    ResourcePtr<RenderPass> render_pass;
    CommandPool<CommandPoolType::Bundle>* command_pool;
};

using RenderPassFunc = std::function<PassTaskResult(RenderPassContext&)>;

template<PassTaskType Type>
struct PassTaskData { };

template<>
struct PassTaskData<PassTaskType::RenderPass> {  
    uint32_t width;
    uint32_t height;
    std::vector<lib::math::Color> clear_colors;
    std::vector<ResourcePtr<GPUTexture>> color_textures;
    std::vector<ResourcePtr<GPUTexture>> input_textures;
    ResourcePtr<GPUTexture> depth_stencil_texture;
    float clear_depth;
    uint8_t clear_stencil;
    RenderPassFunc func;
    ResourcePtr<RenderPass> render_pass;
};

template<>
struct PassTaskData<PassTaskType::ComputePass> { };

template<>
struct PassTaskData<PassTaskType::PresentPass> { 
    ResourcePtr<GPUTexture> swap_texture;
};

struct PassTask {
    std::string name;

    std::variant<
        PassTaskData<PassTaskType::RenderPass>,
        PassTaskData<PassTaskType::ComputePass>,
        PassTaskData<PassTaskType::PresentPass>
    > data;
};

struct RenderPassColorInfo {
    ResourcePtr<GPUTexture> texture;
    backend::RenderPassLoadOp load_op;
    lib::math::Color clear_color;
};

struct RenderPassDepthStencilInfo {
    ResourcePtr<GPUTexture> texture;
    backend::RenderPassLoadOp load_op;
    float clear_depth;
    uint8_t clear_stencil;
};

class FrameGraph {
public:

    FrameGraph(backend::Device& device);
    
    void add_pass(
        std::string_view const name, 
        uint32_t const width,
        uint32_t const height,
        std::span<RenderPassColorInfo const> const color_infos,
        std::span<ResourcePtr<GPUTexture> const> const input_textures,
        std::optional<RenderPassDepthStencilInfo> const depth_stencil_info,
        RenderPassFunc const& func
    );

    void add_present_pass(ResourcePtr<GPUTexture> swap_texture);
    
    void reset();
    
    void execute();

    uint32_t wait();

private:

    backend::Device* _device;

    std::vector<CommandPool<CommandPoolType::Graphics>> _graphics_command_pools;
    std::vector<CommandPool<CommandPoolType::Compute>> _compute_command_pools;
    std::vector<CommandPool<CommandPoolType::Bundle>> _graphics_bundle_command_pools;

    RenderPassCache _render_pass_cache;

    std::vector<PassTask> _tasks;

    uint32_t _frame_index{0};

    std::vector<uint64_t> _fence_values;
};

}
