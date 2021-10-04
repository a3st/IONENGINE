// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DRenderPass : public RenderPass {
public:

    D3DRenderPass(const RenderPassDesc& render_pass_desc) : m_desc(render_pass_desc) {

    }

    const RenderPassDesc& get_desc() const override { return m_desc; }
    
private:

    RenderPassDesc m_desc;
};

}