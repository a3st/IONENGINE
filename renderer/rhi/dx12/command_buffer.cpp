// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "command_buffer.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

auto ionengine::renderer::rhi::render_pass_load_to_d3d12(RenderPassLoadOp const load_op) -> D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE {

    switch(load_op) {
        case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
        case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        default: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    }
}

auto ionengine::renderer::rhi::render_pass_store_to_d3d12(RenderPassStoreOp const store_op) -> D3D12_RENDER_PASS_ENDING_ACCESS_TYPE {

    switch(store_op) {
        case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        default: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    }
}

DX12CommandBuffer::DX12CommandBuffer(
    ID3D12Device1* device, 
    ID3D12CommandAllocator* allocator, 
    winrt::com_ptr<ID3D12GraphicsCommandList4> command_list,
    D3D12_COMMAND_LIST_TYPE const list_type
) :
    allocator(allocator),
    command_list(command_list),
    list_type(list_type)
{
    trackers.reserve(32);
    barriers.reserve(32);
}

auto DX12CommandBuffer::reset() -> void {
    
    THROW_IF_FAILED(command_list->Reset(allocator, nullptr));
}

auto DX12CommandBuffer::close() -> void {
    
    THROW_IF_FAILED(command_list->Close());
}

auto DX12CommandBuffer::set_graphics_pipeline_options(
    core::ref_ptr<Shader> shader,
    RasterizerStageInfo const& rasterizer,
    BlendColorInfo const& blend_color,
    std::optional<DepthStencilStageInfo> const depth_stencil
) -> void 
{

}

auto DX12CommandBuffer::begin_render_pass(
        std::span<RenderPassColorInfo const> const colors,
        std::optional<RenderPassDepthStencilInfo> depth_stencil
) -> void 
{
    auto render_pass_render_targets = std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> {};

    for(uint32_t i = 0; i < colors.size(); ++i) {
        auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        begin.Type = render_pass_load_to_d3d12(colors[i].load_op);

        begin.Clear.ClearValue.Format = texture_format_to_dxgi(colors[i].texture->get_format());
        begin.Clear.ClearValue.Color[0] = colors[i].clear_color.r;
        begin.Clear.ClearValue.Color[1] = colors[i].clear_color.g;
        begin.Clear.ClearValue.Color[2] = colors[i].clear_color.b;
        begin.Clear.ClearValue.Color[3] = colors[i].clear_color.a;
        
        auto end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        end.Type = render_pass_store_to_d3d12(colors[i].store_op);

        render_pass_render_targets[i].BeginningAccess = begin;
        render_pass_render_targets[i].EndingAccess = end;
        render_pass_render_targets[i].cpuDescriptor = static_cast<DX12Texture*>(colors[i].texture.get())->get_descriptor().cpu_handle();

        if(static_cast<DX12Texture*>(colors[i].texture.get())->get_resource_state() != D3D12_RESOURCE_STATE_RENDER_TARGET) {
            auto tracker = ResourceTrackerInfo {
                .resource = static_cast<DX12Texture*>(colors[i].texture.get()),
                .begin_state = D3D12_RESOURCE_STATE_RENDER_TARGET,
                .end_state = static_cast<DX12Texture*>(colors[i].texture.get())->get_resource_state()
            };
            trackers.emplace_back(tracker);
        }
    }

    if(depth_stencil.has_value()) {
        auto value = depth_stencil.value();

        auto render_pass_depth_stencil = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC {};
        {
            auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
            begin.Type = render_pass_load_to_d3d12(value.depth_load_op);
            begin.Clear.ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
            begin.Clear.ClearValue.DepthStencil.Depth = value.clear_depth;

            auto end = D3D12_RENDER_PASS_ENDING_ACCESS {};
            end.Type = render_pass_store_to_d3d12(value.depth_store_op);

            render_pass_depth_stencil.DepthBeginningAccess = begin;
            render_pass_depth_stencil.DepthEndingAccess = end;
            
        }

        {
            auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
            begin.Type = render_pass_load_to_d3d12(value.stencil_load_op);
            begin.Clear.ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
            begin.Clear.ClearValue.DepthStencil.Depth = value.clear_depth;

            auto end = D3D12_RENDER_PASS_ENDING_ACCESS {};
            end.Type = render_pass_store_to_d3d12(value.stencil_store_op);

            render_pass_depth_stencil.StencilBeginningAccess = begin;
            render_pass_depth_stencil.StencilEndingAccess = end;
        }
        render_pass_depth_stencil.cpuDescriptor = static_cast<DX12Texture*>(value.texture.get())->get_descriptor().cpu_handle();

        if(static_cast<DX12Texture*>(value.texture.get())->get_resource_state() != D3D12_RESOURCE_STATE_DEPTH_WRITE) {
            auto tracker = ResourceTrackerInfo {
                .resource = static_cast<DX12Texture*>(value.texture.get()),
                .begin_state = D3D12_RESOURCE_STATE_DEPTH_WRITE,
                .end_state = static_cast<DX12Texture*>(value.texture.get())->get_resource_state()
            };
            trackers.emplace_back(tracker);
        }

        begin_barrier_resources();

        command_list->BeginRenderPass(
            static_cast<uint32_t>(colors.size()), 
            render_pass_render_targets.data(), 
            &render_pass_depth_stencil, 
            D3D12_RENDER_PASS_FLAG_NONE
        );
    } else {
        begin_barrier_resources();

        command_list->BeginRenderPass(
            static_cast<uint32_t>(colors.size()), 
            render_pass_render_targets.data(), 
            nullptr, 
            D3D12_RENDER_PASS_FLAG_NONE
        );
    }
}

auto DX12CommandBuffer::end_render_pass() -> void {
    
    command_list->EndRenderPass();

    end_barrier_resources();
}

auto DX12CommandBuffer::bind_vertex_buffer(uint32_t const slot, core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void {

}

auto DX12CommandBuffer::bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size, IndexFormat const format) -> void {

}

auto DX12CommandBuffer::draw_indexed(uint32_t const index_count, uint32_t const instance_count, uint32_t instance_offset) -> void {

}

auto DX12CommandBuffer::copy_buffer(
    core::ref_ptr<Buffer> dst, 
    uint64_t const dst_offset, 
    core::ref_ptr<Buffer> src, 
    uint64_t const src_offset,
    size_t const size
) -> void 
{
    auto tracker = ResourceTrackerInfo {
        .resource = static_cast<DX12Buffer*>(dst.get()),
        .begin_state = D3D12_RESOURCE_STATE_COPY_DEST,
        .end_state = static_cast<DX12Buffer*>(dst.get())->get_resource_state()
    };
    trackers.emplace_back(tracker);

    begin_barrier_resources();
    command_list->CopyBufferRegion(
        static_cast<DX12Buffer*>(dst.get())->get_resource(), 
        dst_offset,
        static_cast<DX12Buffer*>(src.get())->get_resource(), 
        src_offset,
        size
    );
    end_barrier_resources();
}

auto DX12CommandBuffer::begin_barrier_resources() -> void {

    for(auto const& tracker : trackers) {
        auto d3d12_resource_barrier = D3D12_RESOURCE_BARRIER {};
        d3d12_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3d12_resource_barrier.Transition.StateBefore = tracker.end_state;
        d3d12_resource_barrier.Transition.StateAfter = tracker.begin_state;
        d3d12_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        auto visitor = make_visitor(
            [&](DX12Texture* data) {
                d3d12_resource_barrier.Transition.pResource = data->get_resource();
            },
            [&](DX12Buffer* data) {
                d3d12_resource_barrier.Transition.pResource = data->get_resource();
            }
        );

        std::visit(visitor, tracker.resource);
        barriers.emplace_back(d3d12_resource_barrier);
    }

    command_list->ResourceBarrier(static_cast<uint32_t>(barriers.size()), barriers.data());
    barriers.clear();
}

auto DX12CommandBuffer::end_barrier_resources() -> void {

    for(auto const& tracker : trackers) {
        auto d3d12_resource_barrier = D3D12_RESOURCE_BARRIER {};
        d3d12_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3d12_resource_barrier.Transition.StateBefore = tracker.begin_state;
        d3d12_resource_barrier.Transition.StateAfter = tracker.end_state;
        d3d12_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        auto visitor = make_visitor(
            [&](DX12Texture* data) {
                d3d12_resource_barrier.Transition.pResource = data->get_resource();
            },
            [&](DX12Buffer* data) {
                d3d12_resource_barrier.Transition.pResource = data->get_resource();
            }
        );

        std::visit(visitor, tracker.resource);
        barriers.emplace_back(d3d12_resource_barrier);
    }

    command_list->ResourceBarrier(static_cast<uint32_t>(barriers.size()), barriers.data());
    barriers.clear();

    trackers.clear();
}
