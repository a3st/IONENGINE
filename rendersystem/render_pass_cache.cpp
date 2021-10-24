// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "render_pass_cache.h"

using namespace ionengine::rendersystem;

RenderPassCache::RenderPassCache(lgfx::Device* device) : device_(device) {

}

lgfx::RenderPass* RenderPassCache::GetRenderPass(const lgfx::RenderPassDesc& desc) {

    auto it = render_passes_.find(desc);
    if(it != render_passes_.end()) {
        return it->second.get();
    } else {
        auto result = render_passes_.emplace(desc, std::make_unique<lgfx::RenderPass>(device_, desc));
        return result.first->second.get();
    }
}

void RenderPassCache::Clear() {

    render_passes_.clear();
}