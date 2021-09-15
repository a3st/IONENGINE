// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DRenderPass : public RenderPass {
public:

    D3DRenderPass(winrt::com_ptr<ID3D12Device4>& device, const RenderPassDesc& render_pass_desc) 
        : m_device(device), m_render_pass_desc(render_pass_desc) {

    }

    const RenderPassDesc& get_desc() const override { return m_render_pass_desc; }
    
private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    RenderPassDesc m_render_pass_desc;
};

class D3DFrameBuffer : public FrameBuffer {
public:

    D3DFrameBuffer(winrt::com_ptr<ID3D12Device4>& device, const FrameBufferDesc& frame_buffer_desc) 
        : m_frame_buffer_desc(frame_buffer_desc) {

    }

    const FrameBufferDesc& get_desc() const override { return m_frame_buffer_desc; }

private:

    FrameBufferDesc m_frame_buffer_desc;
};

}