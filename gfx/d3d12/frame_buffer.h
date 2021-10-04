// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<>
class FrameBuffer<backend::d3d12> {
public:

    FrameBuffer(const FrameBufferDesc<backend::d3d12>& frame_buffer_desc) : m_desc(frame_buffer_desc) {

    }

    const FrameBufferDesc<backend::d3d12>& get_desc() const { return m_desc; }

private:

    FrameBufferDesc<backend::d3d12> m_desc;
};

}