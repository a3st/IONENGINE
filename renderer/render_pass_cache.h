// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/resource_ptr.h>
#include <renderer/render_pass.h>

namespace ionengine::renderer {

class RenderPassCache {
public:

    RenderPassCache(backend::Device& device);

    RenderPassCache(RenderPassCache const&) = delete;

    RenderPassCache(RenderPassCache&& other) noexcept;

    RenderPassCache& operator=(RenderPassCache const&) = delete;

    RenderPassCache& operator=(RenderPassCache&& other) noexcept;

    ResourcePtr<RenderPass> get(
        std::span<GPUTexture const*> const colors,
        std::span<backend::RenderPassColorDesc const> const color_descs,
        GPUTexture const* depth_stencil,
        std::optional<backend::RenderPassDepthStencilDesc> const depth_stencil_desc
    );

private:

    backend::Device* _device;
    std::shared_mutex _mutex;
    std::unordered_map<uint64_t, CacheEntry<ResourcePtr<RenderPass>>> _data;
};

uint64_t render_pass_calculate_hash(
    std::span<GPUTexture const*> const colors,
    std::span<backend::RenderPassColorDesc const> const color_descs,
    GPUTexture const* depth_stencil,
    std::optional<backend::RenderPassDepthStencilDesc> const depth_stencil_desc
);

}
