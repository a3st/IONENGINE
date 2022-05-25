// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/resource_ptr.h>
#include <renderer/gpu_texture.h>
#include <renderer/gpu_buffer.h>
#include <lib/math/color.h>
#include <lib/hash/crc32.h>

namespace ionengine::renderer {

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
    backend::Handle<backend::RenderPass> render_pass;
    backend::Handle<backend::CommandList> command_list;
};

using RenderPassFunc = std::function<void(RenderPassContext const&)>;
inline RenderPassFunc RenderPassDefaultFunc = [=](RenderPassContext const& context) { };

struct CreateStorageInfo {
    ResourcePtr<GPUBuffer> buffer;
};

struct ComputePassContext {
    backend::Handle<backend::CommandList> command_list;
};

using ComputePassFunc = std::function<void(ComputePassContext const&)>;
inline ComputePassFunc ComputePassDefaultFunc = [=](ComputePassContext const& context) { };

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
        backend::Handle<backend::CommandList> command_list;
    };

    struct ComputePass {
        std::string name;
        std::vector<ResourcePtr<GPUBuffer>> storages;
        ComputePassFunc func;
        backend::Handle<backend::CommandList> command_list;
    };

    std::vector<backend::Handle<backend::CommandList>> _command_lists;

    backend::Device* _device;

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
    uint32_t _frame_count{2};

    std::vector<uint64_t> _fence_values;

    backend::Handle<backend::Texture> _swapchain_texture;
    backend::MemoryState _swapchain_memory_state;

    backend::Handle<backend::RenderPass> compile_render_pass(
        std::span<ResourcePtr<GPUTexture>> color_attachments,
        std::span<backend::RenderPassLoadOp> color_ops,
        ResourcePtr<GPUTexture> depth_stencil_attachment,
        backend::RenderPassLoadOp depth_stencil_op
    );
};

}
