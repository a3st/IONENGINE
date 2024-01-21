// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/command_buffer.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>

namespace ionengine {

namespace renderer {

namespace rhi {

auto d3d12_to_command_buffer_type(D3D12_COMMAND_LIST_TYPE const list_type) -> CommandBufferType;

class DX12CommandBuffer : public CommandBuffer {
public:

    DX12CommandBuffer(ID3D12Device1* device);

    auto set_graphics_pipeline_options(
        core::ref_ptr<Shader> shader,
        RasterizerStageInfo const& rasterizer,
        BlendColorInfo const& blend_color,
        std::optional<DepthStencilStageInfo> const depth_stencil
    ) -> void override;

    auto begin_render_pass(
        std::span<RenderPassColorInfo const> const colors,
        std::optional<RenderPassDepthStencilInfo> depth_stencil
    ) -> void override;

    auto end_render_pass() -> void override;

    auto bind_vertex_buffer(uint32_t const slot, core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void override;

    auto bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size, IndexFormat const format) -> void override;

    auto draw_indexed(uint32_t const index_count, uint32_t const instance_count, uint32_t instance_offset) -> void override;

private:

    ID3D12GraphicsCommandList4* command_list;
};

}

}

}