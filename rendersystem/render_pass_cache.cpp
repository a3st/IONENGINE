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

lgfx::RenderPass* RenderPassCache::GetRenderPass(const lgfx::RenderPassDesc& desc) {

    auto it = render_passes_.find(desc);
    if(it != render_passes_.end()) {
        return &it->second;
    } else {
        auto result = render_passes_.emplace(desc, lgfx::RenderPass(device_, desc));
        return &result.first->second;
    }
}

void RenderPassCache::Clear() {

    render_passes_.clear();
}