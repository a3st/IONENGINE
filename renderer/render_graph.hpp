// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <webgpu/webgpu.hpp>
#include <core/ref_ptr.hpp>
#include <xxhash/xxhash64.h>

namespace ionengine {

namespace renderer {

class Backend;

struct RGAttachment {
    std::string name;
    wgpu::TextureFormat format;
    uint32_t sample_count;
    
    struct ColorAttachment {
        wgpu::LoadOp load_op;
        wgpu::StoreOp store_op;
        wgpu::Color clear_color;
    };

    struct DepthStencilAttachment {
        wgpu::LoadOp depth_load_op;
        wgpu::StoreOp depth_store_op;
        wgpu::LoadOp stencil_load_op;
        wgpu::StoreOp stencil_store_op;
        float clear_depth;
        uint32_t clear_stencil;
    };

    std::variant<ColorAttachment, DepthStencilAttachment> attachment;

    static auto swapchain(
        wgpu::LoadOp const load_op, 
        wgpu::StoreOp const store_op, 
        wgpu::Color const& color = wgpu::Color(0.0, 0.0, 0.0, 0.0)
    ) -> RGAttachment {
        return RGAttachment {
            .name = "__swapchain__",
            .format = wgpu::TextureFormat::Undefined,
            .sample_count = 1,
            .attachment = ColorAttachment { .load_op = load_op, .store_op = store_op, .clear_color = color }
        };
    }

    static auto render_target(
        std::string_view const name, 
        wgpu::TextureFormat const format, 
        uint32_t const sample_count,
        wgpu::LoadOp const load_op, 
        wgpu::StoreOp const store_op, 
        wgpu::Color const& color = wgpu::Color(0.0, 0.0, 0.0, 0.0)
    ) -> RGAttachment {
        return RGAttachment {
            .name = std::string(name),
            .format = format,
            .sample_count = sample_count,
            .attachment = ColorAttachment { .load_op = load_op, .store_op = store_op, .clear_color = color }
        };
    }
};

struct RGResource {

};

class RGResourceCache {
public:

    struct Entry {
        wgpu::TextureFormat format;
        uint32_t sample_count;
        uint32_t width;
        uint32_t height;

        auto operator==(Entry const& other) const -> bool {
            return std::tie(format, sample_count, width, height) == 
                std::tie(other.format, other.sample_count, other.width, other.height);
        }
    };

    struct EntryHasher {
        auto operator()(const Entry& entry) const -> std::size_t {
            return 
                XXHash64::hash(&entry.format, sizeof(wgpu::TextureFormat), 0) ^
                XXHash64::hash(&entry.sample_count, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.width, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.height, sizeof(uint32_t), 0)
            ;
        }
    };

    RGResourceCache(Backend& backend);

    auto get(wgpu::TextureFormat const format, uint32_t const sample_count, uint32_t const width, uint32_t const height) -> RGResource;

private:

    Backend* backend;
    std::unordered_map<Entry, std::queue<RGResource>, EntryHasher> entries;
};

class RenderGraph : public core::ref_counted_object {
public:

    RenderGraph(Backend& backend);

    auto execute() -> void;

private:

    Backend* backend;
    RGResourceCache resource_cache;
    uint32_t frame_index{0};
};

class RenderGraphBuilder {
public:

    RenderGraphBuilder(Backend& backend);

    auto add_graphics_pass(
        std::string_view const pass_name,
        uint32_t const width,
        uint32_t const height,
        std::span<RGAttachment const> const inputs,
        std::span<RGAttachment const> const outputs
    ) -> RenderGraphBuilder&;

    auto add_compute_pass() -> RenderGraphBuilder&;

    auto build(uint32_t const frame_count) -> core::ref_ptr<RenderGraph>;

private:

    Backend* backend;
};

}

}