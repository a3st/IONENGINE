// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DFrameBuffer : public FrameBuffer {
public:

    D3DFrameBuffer(ID3D12Device4* d3d12_device, const FrameBufferDesc& frame_buffer_desc) : m_desc(frame_buffer_desc) {

    }

    const FrameBufferDesc& get_desc() const override { return m_desc; }

private:

    FrameBufferDesc m_desc;
};

}