// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rg_resource_cache.hpp"
#include "buffer_pool.hpp"

namespace ionengine {

namespace renderer {

struct RGAttachment {
    std::string name;
    rhi::TextureFormat format;
    uint32_t sample_count;
    uint32_t width;
    uint32_t height;
    core::ref_ptr<rhi::Texture> texture{nullptr};

    struct ColorAttachment {
        rhi::RenderPassLoadOp load_op;
        rhi::RenderPassStoreOp store_op;
        math::Color clear_color;
    };

    struct DepthStencilAttachment {
        rhi::RenderPassLoadOp depth_load_op;
        rhi::RenderPassStoreOp depth_store_op;
        rhi::RenderPassLoadOp stencil_load_op;
        rhi::RenderPassStoreOp stencil_store_op;
        float clear_depth;
        uint8_t clear_stencil;
    };

    std::variant<ColorAttachment, DepthStencilAttachment> attachment;

    static auto DepthStencil(
        std::string_view const name, 
        rhi::TextureFormat const format,
        uint32_t const width,
        uint32_t const height,
        rhi::RenderPassLoadOp const depth_load_op, 
        rhi::RenderPassStoreOp const depth_store_op, 
        rhi::RenderPassLoadOp const stencil_load_op, 
        rhi::RenderPassStoreOp const stencil_store_op, 
        float const clear_depth = 1.0f,
        uint8_t const clear_stencil = 0x0
    ) -> RGAttachment {
        return RGAttachment {
            .name = std::string(name),
            .format = format,
            .sample_count = 1,
            .attachment = DepthStencilAttachment { 
                .depth_load_op = depth_load_op, 
                .depth_store_op = depth_store_op, 
                .stencil_load_op = stencil_load_op, 
                .stencil_store_op = stencil_store_op, 
                .clear_depth = clear_depth,
                .clear_stencil = clear_stencil
            }
        };
    }

    static auto Swapchain(
        rhi::RenderPassLoadOp const load_op, 
        rhi::RenderPassStoreOp const store_op, 
        math::Color const& color = math::Color(0.0f, 0.0f, 0.0f, 0.0f)
    ) -> RGAttachment {
        return RGAttachment {
            .name = "__swapchain__",
            .format = rhi::TextureFormat::Unknown,
            .sample_count = 1,
            .attachment = ColorAttachment { .load_op = load_op, .store_op = store_op, .clear_color = color }
        };
    }

    static auto RenderTarget(
        std::string_view const name, 
        rhi::TextureFormat const format, 
        uint32_t const sample_count,
        uint32_t const width,
        uint32_t const height,
        rhi::RenderPassLoadOp const load_op, 
        rhi::RenderPassStoreOp const store_op, 
        math::Color const& color = math::Color(0.0f, 0.0f, 0.0f, 0.0f)
    ) -> RGAttachment {
        return RGAttachment {
            .name = std::string(name),
            .format = format,
            .sample_count = sample_count,
            .width = width,
            .height = height,
            .attachment = ColorAttachment { .load_op = load_op, .store_op = store_op, .clear_color = color }
        };
    }

    static auto External(
        core::ref_ptr<rhi::Texture> texture,
        std::variant<ColorAttachment, DepthStencilAttachment> const attachment
    ) -> RGAttachment {
        return RGAttachment {
            .name = std::format("__external_{}__", (uintptr_t)texture.get()),
            .format = texture->get_format(),
            .sample_count = 1,
            .width = texture->get_width(),
            .height = texture->get_height(),
            .texture = texture,
            .attachment = attachment
        };
    }
};

class RGRenderPassContext {
public:

    RGRenderPassContext(
        std::unordered_map<uint32_t, uint64_t>& inputs,
        std::unordered_map<uint32_t, uint64_t>& outputs,
        std::unordered_map<uint64_t, RGAttachment>& attachments,
        RGResourceCache& resource_cache,
        BufferPool& buffer_pool,
        rhi::CommandBuffer& command_buffer
    );

    auto get_command_buffer() -> rhi::CommandBuffer& {

        return *command_buffer;
    }

    template<typename Type>
    auto bind_buffer(std::string_view const binding, Type const& data, rhi::BufferUsage const usage) -> void {
        
        auto buffer = buffer_pool->allocate(
            sizeof(Type),
            (rhi::BufferUsageFlags)(rhi::BufferUsage::MapWrite | usage),
            std::span<uint8_t const>((uint8_t*)&data, sizeof(Type))
        );

        command_buffer->bind_descriptor(binding, rhi::BufferBindData { buffer, usage });
    }

    auto bind_texture(std::string_view const binding, core::ref_ptr<rhi::Texture> texture, rhi::TextureUsage const usage) -> void {

        command_buffer->bind_descriptor(binding, rhi::TextureBindData { texture, usage });
    }

private:

    std::unordered_map<uint32_t, uint64_t>* inputs;
    std::unordered_map<uint32_t, uint64_t>* outputs;
    std::unordered_map<uint64_t, RGAttachment>* attachments;
    RGResourceCache* resource_cache;
    BufferPool* buffer_pool;
    rhi::CommandBuffer* command_buffer;
};

using graphics_pass_func_t = std::function<void(RGRenderPassContext&)>;

struct RGRenderPass {
    std::string pass_name;
    std::unordered_map<uint32_t, uint64_t> inputs;
    std::unordered_map<uint32_t, uint64_t> outputs;

    struct GraphicsPass {
        uint32_t width;
        uint32_t height;
        std::vector<rhi::RenderPassColorInfo> colors;
        std::optional<rhi::RenderPassDepthStencilInfo> depth_stencil;
        graphics_pass_func_t callback;
    };

    struct ComputePass {

    };

    std::variant<GraphicsPass, ComputePass> pass;

    static auto graphics(
        graphics_pass_func_t callback,
        std::string_view const pass_name, 
        uint32_t const width, 
        uint32_t const height, 
        std::vector<rhi::RenderPassColorInfo> const& colors,
        std::optional<rhi::RenderPassDepthStencilInfo> const depth_stencil = std::nullopt
    ) {
        return RGRenderPass {
            .pass_name = std::string(pass_name),
            .pass = GraphicsPass {
                .width = width,
                .height = height,
                .colors = colors,
                .depth_stencil = depth_stencil,
                .callback = callback
            }
        };
    }
};

class RenderGraph : public core::ref_counted_object {
public:

    RenderGraph(
        rhi::Device& device,
        core::ref_ptr<rhi::MemoryAllocator> allocator,
        std::vector<RGRenderPass> const& steps,
        std::unordered_map<uint64_t, RGAttachment> const& attachments
    );

    auto execute() -> void;

private:

    rhi::Device* device;
    std::vector<core::ref_ptr<BufferPool>> buffer_pools;
    core::ref_ptr<RGResourceCache> resource_cache{nullptr};
    std::vector<RGRenderPass> steps;
    std::unordered_map<uint64_t, RGAttachment> attachments;
    uint32_t frame_index{0};
    std::vector<core::ref_ptr<rhi::CommandBuffer>> submits;
};

class RenderGraphBuilder {
public:

    RenderGraphBuilder() = default;

    auto add_graphics_pass(
        std::string_view const pass_name,
        uint32_t const width,
        uint32_t const height,
        std::span<RGAttachment const> const inputs,
        std::span<RGAttachment const> const outputs,
        graphics_pass_func_t const& callback
    ) -> RenderGraphBuilder&;

    auto add_compute_pass() -> RenderGraphBuilder&;

    auto build(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator) -> core::ref_ptr<RenderGraph>;

private:

    std::vector<RGRenderPass> steps;
    std::unordered_map<uint64_t, RGAttachment> attachments;
};

}

}