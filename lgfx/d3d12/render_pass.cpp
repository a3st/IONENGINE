// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "render_pass.h"
#include "conversion.h"

using namespace lgfx;

RenderPass::RenderPass(Device* device, const RenderPassDesc& desc) : desc_(desc) {

    colors_desc_.resize(desc.colors.size());
    for(uint32_t i = 0; i < static_cast<uint32_t>(desc.colors.size()); ++i) {
        D3D12_RENDER_PASS_BEGINNING_ACCESS begin{};
        begin.Type = ToD3D12RenderPassBeginType(desc.colors[i].load_op);

        begin.Clear.ClearValue.Format = ToDXGIFormat(desc.colors[i].format);

        D3D12_RENDER_PASS_ENDING_ACCESS end{};
        end.Type = ToD3D12RenderPassEndType(desc.colors[i].store_op);

        colors_desc_[i].BeginningAccess = begin;
        colors_desc_[i].EndingAccess = end;
    }

    {
        D3D12_RENDER_PASS_BEGINNING_ACCESS depth_begin{};
        depth_begin.Type = ToD3D12RenderPassBeginType(desc.depth_stencil.depth_load_op);

        depth_begin.Clear.ClearValue.Format = ToDXGIFormat(desc.depth_stencil.format);

        D3D12_RENDER_PASS_ENDING_ACCESS depth_end{};
        depth_end.Type = ToD3D12RenderPassEndType(desc.depth_stencil.depth_store_op);

        D3D12_RENDER_PASS_BEGINNING_ACCESS stencil_begin{};
        stencil_begin.Type = ToD3D12RenderPassBeginType(desc.depth_stencil.stencil_load_op);

        stencil_begin.Clear.ClearValue.Format = ToDXGIFormat(desc.depth_stencil.format);

        D3D12_RENDER_PASS_ENDING_ACCESS stencil_end{};
        stencil_end.Type = ToD3D12RenderPassEndType(desc.depth_stencil.depth_store_op);

        depth_stencil_desc_.DepthBeginningAccess = depth_begin;
        depth_stencil_desc_.DepthEndingAccess = depth_end;
        depth_stencil_desc_.StencilBeginningAccess = stencil_begin;
        depth_stencil_desc_.StencilEndingAccess = stencil_end;
    }
}