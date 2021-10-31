// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class FrameBufferCache {

public:

    struct Key {

        lgfx::RenderPass* render_pass;
        uint32_t width;
        uint32_t height;
        std::vector<lgfx::TextureView*> colors;
        lgfx::TextureView* depth_stencil;

        inline bool operator<(const Key& rhs) const {

            return std::tie(render_pass, width, height, colors, depth_stencil) <
                std::tie(rhs.render_pass, rhs.width, rhs.height, rhs.colors, rhs.depth_stencil);
        }
    };

    FrameBufferCache(lgfx::Device* device);
    FrameBufferCache(const FrameBufferCache&) = delete;
    FrameBufferCache(FrameBufferCache&&) = delete;

    FrameBufferCache& operator=(const FrameBufferCache&) = delete;
    FrameBufferCache& operator=(FrameBufferCache&&) = delete;

    lgfx::FrameBuffer* GetFrameBuffer(const Key& key);
    
    inline void Reset() {

        frame_buffers_.clear();
    }

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::FrameBuffer>> frame_buffers_;
};

}