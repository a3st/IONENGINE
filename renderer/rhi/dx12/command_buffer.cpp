// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "command_buffer.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

auto ionengine::renderer::rhi::d3d12_to_command_buffer_type(D3D12_COMMAND_LIST_TYPE const list_type) -> CommandBufferType {

    switch(list_type) {
        case D3D12_COMMAND_LIST_TYPE_DIRECT: return CommandBufferType::Graphics;
        case D3D12_COMMAND_LIST_TYPE_COPY: return CommandBufferType::Copy;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: return CommandBufferType::Compute;
        default: return CommandBufferType::Graphics;
    }
}

DX12CommandBuffer::DX12CommandBuffer(ID3D12Device1* device) {

}

auto DX12CommandBuffer::set_graphics_pipeline_options(
    core::ref_ptr<Shader> shader,
    RasterizerStageInfo const& rasterizer,
    BlendColorInfo const& blend_color,
    std::optional<DepthStencilStageInfo> const depth_stencil
) -> void {

}

auto DX12CommandBuffer::begin_render_pass(
        std::span<RenderPassColorInfo const> const colors,
        std::optional<RenderPassDepthStencilInfo> depth_stencil
) -> void {

}

auto DX12CommandBuffer::end_render_pass() -> void {

}

auto DX12CommandBuffer::bind_vertex_buffer(uint32_t const slot, core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void {

}

auto DX12CommandBuffer::bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size, IndexFormat const format) -> void {

}

auto DX12CommandBuffer::draw_indexed(uint32_t const index_count, uint32_t const instance_count, uint32_t instance_offset) -> void {

}