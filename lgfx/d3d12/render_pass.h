// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class RenderPass {

friend class CommandBuffer;
friend class Pipeline;

public:

    RenderPass() = default;

    RenderPass(
        Device* const device, 
        const std::span<const RenderPassColorDesc> colors, 
        const RenderPassDepthStencilDesc& depth_stencil, 
        const uint32_t sample_count = 1);

    RenderPass(const RenderPass&) = delete;
    RenderPass(RenderPass&& rhs) noexcept = default;

    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&& rhs) noexcept = default;

private:

    std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> render_pass_target_descs_;
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC render_pass_depth_stencil_desc_;

    std::vector<RenderPassColorDesc> colors_;
    RenderPassDepthStencilDesc depth_stencil_;
    uint32_t sample_count_;
};

}