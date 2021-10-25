// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class FrameBufferCache {

    using Key = lgfx::FrameBufferDesc;

public:

    FrameBufferCache(lgfx::Device* device);

    lgfx::FrameBuffer* GetFrameBuffer(const Key& key);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::FrameBuffer>> frame_buffers_;
};

}