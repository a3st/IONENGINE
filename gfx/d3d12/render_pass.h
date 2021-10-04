// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<>
class RenderPass<backend::d3d12> {
public:

    RenderPass(const RenderPassDesc& render_pass_desc) : m_desc(render_pass_desc) {

    }

    const RenderPassDesc& get_desc() const { return m_desc; }
    
private:

    RenderPassDesc m_desc;
};

}