// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "pipeline_cache.hpp"
#include "renderer/rhi/command_buffer.hpp"
#include "texture.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::renderer::rhi
{
    auto d3d12_to_command_buffer_type(D3D12_COMMAND_LIST_TYPE const list_type) -> CommandBufferType;

    auto render_pass_load_to_d3d12(RenderPassLoadOp const load_op) -> D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE;

    auto render_pass_store_to_d3d12(RenderPassStoreOp const store_op) -> D3D12_RENDER_PASS_ENDING_ACCESS_TYPE;

    class DX12CommandBuffer : public CommandBuffer
    {
      public:
        DX12CommandBuffer(ID3D12Device1* device, ID3D12CommandAllocator* allocator, PipelineCache* pipeline_cache,
                          DescriptorAllocator* descriptor_allocator,
                          winrt::com_ptr<ID3D12GraphicsCommandList4> command_list,
                          D3D12_COMMAND_LIST_TYPE const list_type);

        auto reset() -> void;

        auto close() -> void override;

        auto set_graphics_pipeline_options(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                           BlendColorInfo const& blend_color,
                                           std::optional<DepthStencilStageInfo> const depth_stencil) -> void override;

        auto bind_descriptor(std::string_view const binding, std::variant<BufferBindData, TextureBindData> data)
            -> void override;

        auto begin_render_pass(std::span<RenderPassColorInfo> const colors,
                               std::optional<RenderPassDepthStencilInfo> depth_stencil) -> void override;

        auto end_render_pass() -> void override;

        auto bind_vertex_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
            -> void override;

        auto bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                               IndexFormat const format) -> void override;

        auto draw_indexed(uint32_t const index_count, uint32_t const instance_count, uint32_t const instance_offset)
            -> void override;

        auto draw(uint32_t const vertex_count, uint32_t const instance_count, uint32_t const instance_offset)
            -> void override;

        auto copy_buffer(core::ref_ptr<Buffer> dst, uint64_t const dst_offset, core::ref_ptr<Buffer> src,
                         uint64_t const src_offset, size_t const size) -> void override;

        auto copy_buffer(core::ref_ptr<Texture> dst, core::ref_ptr<Buffer> src,
                         std::span<TextureCopyRegion const> const regions) -> void override;

        auto set_viewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
            -> void override;

        auto set_scissor(int32_t const left, int32_t const top, int32_t const right, int32_t const bottom)
            -> void override;

        auto get_command_list() -> ID3D12GraphicsCommandList4*
        {
            return command_list.get();
        }

        auto get_list_type() const -> D3D12_COMMAND_LIST_TYPE
        {
            return list_type;
        }

      private:
        ID3D12CommandAllocator* allocator;
        PipelineCache* pipeline_cache;
        DescriptorAllocator* descriptor_allocator;
        winrt::com_ptr<ID3D12GraphicsCommandList4> command_list;
        D3D12_COMMAND_LIST_TYPE list_type;
        core::ref_ptr<DX12Shader> current_shader;
        bool is_root_signature_binded;
        std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> render_target_formats;
        DXGI_FORMAT depth_stencil_format;
        std::array<DX12Texture*, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> render_target_textures;
        DX12Texture* depth_stencil_texture;
        std::array<uint32_t, 16> bindings;
    };
} // namespace ionengine::renderer::rhi