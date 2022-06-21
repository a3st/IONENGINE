// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/render_pass_cache.h>
#include <renderer/gpu_texture.h>

using namespace ionengine;
using namespace ionengine::renderer;

RenderPassCache::RenderPassCache(backend::Device& device) :
    _device(&device) {
}

RenderPassCache::RenderPassCache(RenderPassCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
}

RenderPassCache& RenderPassCache::operator=(RenderPassCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
    return *this;
}

uint64_t ionengine::renderer::render_pass_calculate_hash(
    std::span<GPUTexture const*> const colors,
    std::span<backend::RenderPassColorDesc const> const color_descs,
    GPUTexture const* depth_stencil,
    std::optional<backend::RenderPassDepthStencilDesc> const depth_stencil_desc
) {
    uint32_t color_hash = colors[0]->texture.index() + colors[0]->texture.generation();

    for(size_t i = 1; i < colors.size(); ++i) {
        color_hash ^= colors[i]->texture.index() + colors[i]->texture.generation();
    }

    uint32_t color_desc_hash = static_cast<uint32_t>(color_descs[0].load_op) ^ static_cast<uint32_t>(color_descs[0].store_op);

    for(size_t i = 1; i < color_descs.size(); ++i) {
        color_desc_hash ^= static_cast<uint32_t>(color_descs[i].load_op) ^ static_cast<uint32_t>(color_descs[i].store_op);
    }

    uint32_t low = std::numeric_limits<uint32_t>::max();

    if(depth_stencil) {
        uint32_t const depth_stencil_hash = depth_stencil->texture.index() ^ depth_stencil->texture.generation();

        uint32_t const depth_stencil_desc_hash = 
            static_cast<uint32_t>(depth_stencil_desc.value().depth_load_op) ^
            static_cast<uint32_t>(depth_stencil_desc.value().depth_store_op) ^
            static_cast<uint32_t>(depth_stencil_desc.value().stencil_load_op) ^
            static_cast<uint32_t>(depth_stencil_desc.value().stencil_store_op)
        ;

        low = depth_stencil_hash ^ depth_stencil_desc_hash;
    }

    uint32_t const high = color_hash ^ color_desc_hash;

    return ((static_cast<uint64_t>(high) << 32) | static_cast<uint64_t>(low));
}

ResourcePtr<RenderPass> RenderPassCache::get(
    std::span<GPUTexture const*> const colors,
    std::span<backend::RenderPassColorDesc const> const color_descs,
    GPUTexture const* depth_stencil,
    std::optional<backend::RenderPassDepthStencilDesc> const depth_stencil_desc
) {
    uint64_t hash;
    {
        std::shared_lock lock(_mutex);
        hash = render_pass_calculate_hash(colors, color_descs, depth_stencil, depth_stencil_desc);

        auto it = _data.find(hash);

        if(it != _data.end()) {
            return it->second.value;
        }
    }

    std::lock_guard lock(_mutex);

    auto result = RenderPass::create(*_device, colors, color_descs, depth_stencil, depth_stencil_desc);

    if(result.is_ok()) {

        RenderPass render_pass = std::move(result.as_ok());

        auto cache_entry = CacheEntry<ResourcePtr<RenderPass>> {
            .value = make_resource_ptr(std::move(render_pass)),
            .hash = hash
        };
        
        return _data.insert({ hash, std::move(cache_entry) }).first->second.value;
    } else {
        throw lib::Exception(result.as_error().message);
    }
}
