// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <renderer/gpu_texture.h>
#include <renderer/resource_ptr.h>
#include <renderer/command_pool.h>
#include <lib/math/color.h>
#include <lib/expected.h>

namespace ionengine::renderer {

struct RenderPass;

enum class PassTaskType {
    Single,
    Multithreading
};

enum class RenderPassError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

template<PassTaskType Type>
struct PassTaskData { };

template<>
struct PassTaskData<PassTaskType::Single> {  
    ResourcePtr<CommandList> command_list;
};

template<>
struct PassTaskData<PassTaskType::Multithreading> {  
    std::vector<ResourcePtr<CommandList>> command_lists;
};

struct PassTaskCreateInfo {
    std::variant<
        PassTaskData<PassTaskType::Single>,
        PassTaskData<PassTaskType::Multithreading>
    > data;

    static PassTaskCreateInfo singlethread(ResourcePtr<CommandList>&& command_list) {
        return PassTaskCreateInfo { .data = PassTaskData<PassTaskType::Single> { .command_list = std::move(command_list) } };
    }

    static PassTaskCreateInfo multithread(std::vector<ResourcePtr<CommandList>>&& command_lists) {
        return PassTaskCreateInfo { .data = PassTaskData<PassTaskType::Multithreading> { .command_lists = std::move(command_lists) } };
    }
};

struct RenderPassContext {
    ResourcePtr<RenderPass> render_pass;
    CommandPool<CommandPoolType::Bundle>* command_pool;
};

using RenderPassFunc = std::function<PassTaskCreateInfo(RenderPassContext&)>;

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

struct SwapchainTexture {
    backend::Handle<backend::Texture> texture;
    backend::MemoryState memory_state;
};

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

    static lib::Expected<RenderPass, lib::Result<RenderPassError>> create(
        backend::Device& device,
        std::string_view const name,
        uint32_t const width,
        uint32_t const height,
        std::optional<std::span<CreateColorInfo const>> const colors,
        std::optional<std::span<CreateInputInfo const>> const inputs,
        std::optional<CreateDepthStencilInfo> const depth_stencil,
        RenderPassFunc const& func,
        SwapchainTexture const& swapchain
    );
};

template<>
struct GPUResourceDeleter<RenderPass> {
    void operator()(backend::Device& device, RenderPass const& render_pass) const {
        device.delete_render_pass(render_pass.render_pass);
    }
};

backend::Handle<backend::RenderPass> create_render_pass(
    backend::Device& device,
    std::span<ResourcePtr<GPUTexture>> color_attachments,
    std::span<backend::RenderPassLoadOp> color_ops,
    ResourcePtr<GPUTexture> depth_stencil_attachment,
    backend::RenderPassLoadOp depth_stencil_op,
    SwapchainTexture const& swapchain
);

}
