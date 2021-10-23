// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class FrameBufferCache {

public:

    struct Key {
        lgfx::RenderPass* render_pass;
        uint32_t width;
        uint32_t height;
        std::vector<lgfx::TextureView*> colors;
        lgfx::TextureView* depth_stencil;

        bool operator<(const Key& rhs) const {

            return std::tie(render_pass, width, height, colors, depth_stencil) < std::tie(rhs.render_pass, rhs.width, rhs.height, rhs.colors, rhs.depth_stencil);
        }
    };

    FrameBufferCache();
    FrameBufferCache(lgfx::Device* device);
    FrameBufferCache(const FrameBufferCache&) = delete;
    FrameBufferCache(FrameBufferCache&& rhs) noexcept;

    FrameBufferCache& operator=(const FrameBufferCache&) = delete;
    FrameBufferCache& operator=(FrameBufferCache&& rhs) noexcept;

    lgfx::FrameBuffer* GetFrameBuffer(const Key& key);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<FrameBufferCache::Key, lgfx::FrameBuffer> frame_buffers_;
};

}