// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "render_pass_cache.h"

using namespace ionengine::rendersystem;

RenderPassCache::RenderPassCache() {

}

RenderPassCache::RenderPassCache(lgfx::Device* device) : device_(device) {

}

RenderPassCache::RenderPassCache(RenderPassCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(render_passes_, rhs.render_passes_);
}

RenderPassCache& RenderPassCache::operator=(RenderPassCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(render_passes_, rhs.render_passes_);
    return *this;
}

lgfx::RenderPass* RenderPassCache::GetRenderPass(const Key& key) {

    auto it = render_passes_.find(key);
    if(it != render_passes_.end()) {
        return &it->second;
    } else {
        lgfx::RenderPassDesc desc{};
        desc.colors = key.colors;
        desc.depth_stencil = key.depth_stencil;
        desc.sample_count = key.sample_count;

        auto result = render_passes_.emplace(key, lgfx::RenderPass(device_, desc));
        return &result.first->second;
    } 
}

void RenderPassCache::Clear() {

    render_passes_.clear();
}