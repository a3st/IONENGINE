// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "render_pass.h"
#include "conversion.h"

using namespace lgfx;

RenderPass::RenderPass(Device* device, const std::span<const RenderPassColorDesc> colors, const RenderPassDepthStencilDesc& depth_stencil, uint32_t sample_count) : 
    colors_(colors.begin(), colors.end()), depth_stencil_(depth_stencil), sample_count_(sample_count) {

    render_pass_target_descs_.resize(colors_.size());
    for(size_t i : std::views::iota(0u, colors_.size())) {
        D3D12_RENDER_PASS_BEGINNING_ACCESS begin{};
        begin.Type = ToD3D12RenderPassBeginType(colors_[i].load_op);

        begin.Clear.ClearValue.Format = ToDXGIFormat(colors_[i].format);

        D3D12_RENDER_PASS_ENDING_ACCESS end{};
        end.Type = ToD3D12RenderPassEndType(colors_[i].store_op);

        render_pass_target_descs_[i].BeginningAccess = begin;
        render_pass_target_descs_[i].EndingAccess = end;
    }

    {
        D3D12_RENDER_PASS_BEGINNING_ACCESS depth_begin{};
        depth_begin.Type = ToD3D12RenderPassBeginType(depth_stencil_.depth_load_op);

        depth_begin.Clear.ClearValue.Format = ToDXGIFormat(depth_stencil_.format);

        D3D12_RENDER_PASS_ENDING_ACCESS depth_end{};
        depth_end.Type = ToD3D12RenderPassEndType(depth_stencil_.depth_store_op);

        D3D12_RENDER_PASS_BEGINNING_ACCESS stencil_begin{};
        stencil_begin.Type = ToD3D12RenderPassBeginType(depth_stencil_.stencil_load_op);

        stencil_begin.Clear.ClearValue.Format = ToDXGIFormat(depth_stencil_.format);

        D3D12_RENDER_PASS_ENDING_ACCESS stencil_end{};
        stencil_end.Type = ToD3D12RenderPassEndType(depth_stencil_.depth_store_op);

        render_pass_depth_stencil_desc_.DepthBeginningAccess = depth_begin;
        render_pass_depth_stencil_desc_.DepthEndingAccess = depth_end;
        render_pass_depth_stencil_desc_.StencilBeginningAccess = stencil_begin;
        render_pass_depth_stencil_desc_.StencilEndingAccess = stencil_end;
    }
}

RenderPass::RenderPass(RenderPass&& rhs) noexcept {

    std::swap(render_pass_target_descs_, rhs.render_pass_target_descs_);
    std::swap(render_pass_depth_stencil_desc_, rhs.render_pass_depth_stencil_desc_);
    std::swap(colors_, rhs.colors_);
    std::swap(depth_stencil_, rhs.depth_stencil_);
    std::swap(sample_count_, rhs.sample_count_);
}

RenderPass& RenderPass::operator=(RenderPass&& rhs) noexcept {

    std::swap(render_pass_target_descs_, rhs.render_pass_target_descs_);
    std::swap(render_pass_depth_stencil_desc_, rhs.render_pass_depth_stencil_desc_);
    std::swap(colors_, rhs.colors_);
    std::swap(depth_stencil_, rhs.depth_stencil_);
    std::swap(sample_count_, rhs.sample_count_);
    return *this;
}