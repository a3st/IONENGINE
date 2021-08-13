// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class CommandList {
public:

    virtual ~CommandList() = default;
    
    virtual void bind_pipeline(Pipeline& pipeline) = 0;
    virtual void close() = 0;
    virtual void reset() = 0;
    virtual void set_viewport(const int32 x, const int32 y, const uint32 width, const uint32 height) = 0;
    virtual void set_scissor_rect(int32 left, int32 top, int32 right, int32 bottom) = 0;
    virtual void resource_barriers(const std::vector<ResourceBarrierDesc>& barrier_descs) = 0;
    virtual void begin_render_pass(RenderPass& render_pass, FrameBuffer& frame_buffer, const ClearValueDesc& clear_value_desc) = 0;
    virtual void end_render_pass() = 0;
};

}