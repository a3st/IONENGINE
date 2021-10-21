

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class RenderPassCache {

public:

    struct Key {
        std::vector<lgfx::RenderPassColorDesc> colors;
        lgfx::RenderPassDepthStencilDesc depth_stencil;
        uint32_t sample_count;

        bool operator<(const Key& rhs) const {

            return std::tie(colors, depth_stencil, sample_count) < std::tie(rhs.colors, rhs.depth_stencil, rhs.sample_count);
        }
    };

    RenderPassCache();
    RenderPassCache(lgfx::Device* device);
    RenderPassCache(const RenderPassCache&) = delete;
    RenderPassCache(RenderPassCache&& rhs) noexcept;

    RenderPassCache& operator=(const RenderPassCache&) = delete;
    RenderPassCache& operator=(RenderPassCache&& rhs) noexcept;

    lgfx::RenderPass* GetRenderPass(const Key& key);
    void Clear();

private:

    std::map<RenderPassCache::Key, lgfx::RenderPass> render_passes_;
};

}