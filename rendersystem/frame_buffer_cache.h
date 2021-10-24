// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class FrameBufferCache {

public:

    FrameBufferCache();
    FrameBufferCache(lgfx::Device* device);
    FrameBufferCache(const FrameBufferCache&) = delete;
    FrameBufferCache(FrameBufferCache&& rhs) noexcept;

    FrameBufferCache& operator=(const FrameBufferCache&) = delete;
    FrameBufferCache& operator=(FrameBufferCache&& rhs) noexcept;

    lgfx::FrameBuffer* GetFrameBuffer(const lgfx::FrameBufferDesc& desc);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<lgfx::FrameBufferDesc, std::unique_ptr<lgfx::FrameBuffer>> frame_buffers_;
};

}