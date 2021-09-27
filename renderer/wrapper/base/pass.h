// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class RenderPass {
public:
    virtual ~RenderPass() = default;
    virtual const RenderPassDesc& get_desc() const = 0;
};

class FrameBuffer {
public:
    virtual ~FrameBuffer() = default;
    virtual const FrameBufferDesc& get_desc() const = 0;
};

}