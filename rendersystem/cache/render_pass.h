// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class RenderPassCache {

public:

    struct Key {

        std::vector<lgfx::RenderPassColorDesc> colors;
        lgfx::RenderPassDepthStencilDesc depth_stencil;
        uint32_t sample_count;

        inline bool operator<(const Key& rhs) const {

            return std::tie(colors, depth_stencil, sample_count) < std::tie(rhs.colors, rhs.depth_stencil, rhs.sample_count);
        }
    };

    RenderPassCache(lgfx::Device* device);
    RenderPassCache(const RenderPassCache&) = delete;
    RenderPassCache(RenderPassCache&&) = delete;

    RenderPassCache& operator=(const RenderPassCache&) = delete;
    RenderPassCache& operator=(RenderPassCache&&) = delete;

    lgfx::RenderPass* GetRenderPass(const std::span<const lgfx::RenderPassColorDesc> colors, const lgfx::RenderPassDepthStencilDesc& depth_stencil, const uint32_t sample_count);

    inline void Reset() {

        render_passes_.clear();
    }

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::RenderPass>> render_passes_;
};

}