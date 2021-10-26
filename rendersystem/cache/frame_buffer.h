// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class FrameBufferCache {

    using Key = lgfx::FrameBufferDesc;

public:

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