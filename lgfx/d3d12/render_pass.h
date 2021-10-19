// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class RenderPass {

friend class CommandBuffer;

public:

    RenderPass();
    ~RenderPass();
    RenderPass(Device* device, const RenderPassDesc& desc);
    RenderPass(const RenderPass&) = delete;
    RenderPass(RenderPass&& rhs) noexcept;

    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&& rhs) noexcept;

    inline const RenderPassDesc& GetDesc() const { return desc_; }

private:

    RenderPassDesc desc_;

    std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> colors_desc_;
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depth_stencil_desc_;
};

}