// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/render_pass.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

enum class PassTaskType {
    RenderPass,
    ComputePass
};

enum class PassTaskResultType {
    Single,
    Multithreading
};

template<PassTaskType Type>
struct PassTaskData { };

template<>
struct PassTaskData<PassTaskType::RenderPass> {  
    uint32_t width;
    uint32_t height;
    std::vector<ResourcePtr<GPUTexture>> colors;
    std::vector<lib::math::Color> color_clears;
    std::vector<ResourcePtr<GPUTexture>> inputs;
    ResourcePtr<GPUTexture> depth_stencil;
    std::pair<float, uint8_t> depth_stencil_clear;
    //std::vector<backend::RenderPassLoadOp> color_ops;
    //backend::RenderPassLoadOp ds_op;
    RenderPassFunc func;
    ResourcePtr<RenderPass> render_pass;
};

template<>
struct PassTaskData<PassTaskType::ComputePass> { };

struct PassTask {
    std::string name;

    std::variant<
        PassTaskData<PassTaskType::RenderPass>,
        PassTaskData<PassTaskType::ComputePass>
    > data;
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

class FrameGraph {
public:

    FrameGraph(backend::Device& device);
    
    void add_pass(
        std::string_view const name, 
        uint32_t const width,
        uint32_t const height,
        std::optional<std::span<CreateColorInfo const>> const colors,
        std::optional<std::span<CreateInputInfo const>> const inputs,
        std::optional<CreateDepthStencilInfo> const depth_stencil,
        RenderPassFunc const& func,
        RenderPassHash& cache
    );
    
    void reset();
    
    void execute();

    uint32_t wait();

private:

    backend::Device* _device;

    std::vector<CommandPool<CommandPoolType::Graphics>> _graphics_command_pools;
    std::vector<CommandPool<CommandPoolType::Compute>> _compute_command_pools;
    std::vector<CommandPool<CommandPoolType::Bundle>> _graphics_bundle_command_pools;

    std::unordered_map<RenderPassHash, ResourcePtr<RenderPass>> _render_pass_cache;

    std::vector<PassTask> _tasks;

    uint32_t _frame_index{0};

    std::vector<uint64_t> _fence_values;

    SwapchainTexture _swapchain;
};

}
