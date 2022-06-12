// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/render_pass.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

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
        uint64_t& cache
    );
    
    void reset();
    
    void execute();

    uint32_t wait();

private:

    backend::Device* _device;

    std::vector<CommandPool<CommandPoolType::Graphics>> _graphics_command_pools;
    std::vector<CommandPool<CommandPoolType::Compute>> _compute_command_pools;
    std::vector<CommandPool<CommandPoolType::Bundle>> _graphics_bundle_command_pools;

    std::unordered_map<uint64_t, ResourcePtr<RenderPass>> _render_pass_cache;

    std::vector<ResourcePtr<RenderPass>> _render_passes;

    enum class OpType {
        RenderPass,
        ComputePass
    };

    struct OpData {
        OpType op_type;
        size_t index;
    };

    std::vector<OpData> _ops;

    uint32_t _frame_index{0};

    std::vector<uint64_t> _fence_values;

    SwapchainTexture _swapchain;
};

}
