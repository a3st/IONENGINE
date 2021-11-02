// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "render_pass.h"

using namespace ionengine::rendersystem;

RenderPassCache::RenderPassCache(lgfx::Device* device) : device_(device) {

}

lgfx::RenderPass* RenderPassCache::GetRenderPass(const std::span<const lgfx::RenderPassColorDesc> colors, const lgfx::RenderPassDepthStencilDesc& depth_stencil, const uint32_t sample_count) {

    static RenderPassCache::Key kRenderPassKey{};
    kRenderPassKey.colors.resize(colors.size());
    std::memcpy(kRenderPassKey.colors.data(), colors.data(), sizeof(lgfx::RenderPassColorDesc) * colors.size());
    kRenderPassKey.depth_stencil = depth_stencil;
    kRenderPassKey.sample_count = sample_count;

    auto it = render_passes_.find(kRenderPassKey);
    if(it != render_passes_.end()) {
        return it->second.get();
    } else {
        auto ret = render_passes_.emplace(kRenderPassKey, std::make_unique<lgfx::RenderPass>(device_,
            kRenderPassKey.colors, kRenderPassKey.depth_stencil, kRenderPassKey.sample_count));
        return ret.first->second.get();
    }
}