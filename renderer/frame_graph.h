// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/resource_ptr.h>
#include <renderer/gpu_texture.h>
#include <renderer/gpu_buffer.h>
#include <renderer/command_pool.h>
#include <lib/math/color.h>
#include <lib/hash/crc32.h>
#include <lib/thread_pool.h>

namespace ionengine::renderer {

struct RenderPass;
struct SwapchainTexture;

enum class TaskExecution {
    Single,
    Multithreading
};

struct CreateColorInfo {
    ResourcePtr<GPUTexture> attachment;
    backend::RenderPassLoadOp load_op;
    lib::math::Color clear_color;
};

struct CreateInputInfo {
    ResourcePtr<GPUTexture> attachment;
};

struct CreateDepthStencilInfo {
    ResourcePtr<GPUTexture> attachment;
    backend::RenderPassLoadOp load_op;
    float clear_depth;
    uint8_t clear_stencil;
};

struct RenderPassContext {
    RenderPass const* render_pass;
    CommandPool<CommandPoolType::Bundle>* command_pool;
    lib::ThreadPool* thread_pool;
    backend::Device* device;
    SwapchainTexture* swapchain;

    std::vector<ResourcePtr<CommandList>> single_for(
        std::function<void(backend::Handle<backend::CommandList> const&, backend::Handle<backend::RenderPass> const&)> func
    );
};

struct TaskCreateInfo {
    TaskExecution task_execution;
    std::vector<ResourcePtr<CommandList>> command_lists;
};

using RenderPassFunc = std::function<TaskCreateInfo(RenderPassContext&)>;

struct CreateStorageInfo {
    ResourcePtr<GPUBuffer> buffer;
};

struct ComputePassContext {
    uint16_t thread_index;
    backend::Handle<backend::CommandList> command_list;
};

using ComputePassFunc = std::function<void(ComputePassContext const&)>;
inline ComputePassFunc ComputePassDefaultFunc = [=](ComputePassContext const& context) { };

struct RenderPass {
    std::string name;
    uint32_t width;
    uint32_t height;
    std::vector<ResourcePtr<GPUTexture>> color_attachments;
    std::vector<backend::RenderPassLoadOp> color_ops;
    std::vector<lib::math::Color> color_clears;
    ResourcePtr<GPUTexture> ds_attachment;
    backend::RenderPassLoadOp ds_op;
    std::pair<float, uint8_t> ds_clear;
    std::vector<ResourcePtr<GPUTexture>> inputs;
    RenderPassFunc func;
    backend::Handle<backend::RenderPass> render_pass;
    uint64_t hash;
};

struct SwapchainTexture {
    backend::Handle<backend::Texture> texture;
    backend::MemoryState memory_state;
};

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
        RenderPassFunc const& func
    );

    void add_pass(
        std::string_view const name, 
        std::optional<std::span<CreateStorageInfo const>> const storages,
        ComputePassFunc const& func
    );
    
    void reset();
    
    void execute();

    uint32_t wait();

private:

    struct ComputePass {
        std::string name;
        std::vector<ResourcePtr<GPUBuffer>> storages;
        ComputePassFunc func;
    };

    backend::Device* _device;

    std::vector<CommandPool<CommandPoolType::Graphics>> _graphics_command_pools;
    std::vector<CommandPool<CommandPoolType::Compute>> _compute_command_pools;
    std::vector<CommandPool<CommandPoolType::Bundle>> _graphics_bundle_command_pools;

    std::unordered_map<uint32_t, RenderPass> _render_pass_cache;
    std::unordered_map<uint32_t, ComputePass> _compute_pass_cache;

    std::vector<RenderPass*> _render_passes;
    std::vector<ComputePass*> _compute_passes;

    enum class OpType {
        RenderPass,
        ComputePass,
        Submit
    };

    struct OpData {
        OpType op_type;
        size_t index;
    };

    std::vector<OpData> _ops;

    uint32_t _frame_index{0};

    std::vector<uint64_t> _fence_values;

    SwapchainTexture _swapchain;

    backend::Handle<backend::RenderPass> compile_render_pass(
        std::span<ResourcePtr<GPUTexture>> color_attachments,
        std::span<backend::RenderPassLoadOp> color_ops,
        ResourcePtr<GPUTexture> depth_stencil_attachment,
        backend::RenderPassLoadOp depth_stencil_op
    );
};

}
