// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class FrameBufferCache {

public:

    struct Key {
        

        bool operator<(const Key& rhs) const {

            return false;
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

    std::map<FrameBufferCache::Key, lgfx::FrameBuffer> frame_buffers_;
};

}