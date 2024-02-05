// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "command_buffer.hpp"
#include "barrier_proxy.hpp"
#include "core/exception.hpp"
#include "precompiled.h"
#include "utils.hpp"

namespace ionengine::renderer::rhi
{
    auto ionengine::renderer::rhi::render_pass_load_to_d3d12(RenderPassLoadOp const load_op)
        -> D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE
    {
        switch (load_op)
        {
            case RenderPassLoadOp::Clear:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            case RenderPassLoadOp::Load:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
            case RenderPassLoadOp::DontCare:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
            default:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }
    }

    auto ionengine::renderer::rhi::render_pass_store_to_d3d12(RenderPassStoreOp const store_op)
        -> D3D12_RENDER_PASS_ENDING_ACCESS_TYPE
    {
        switch (store_op)
        {
            case RenderPassStoreOp::Store:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
            case RenderPassStoreOp::DontCare:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
            default:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }
    }

    DX12CommandBuffer::DX12CommandBuffer(ID3D12Device1* device, ID3D12CommandAllocator* allocator,
                                         PipelineCache* pipeline_cache, DescriptorAllocator* descriptor_allocator,
                                         winrt::com_ptr<ID3D12GraphicsCommandList4> command_list,
                                         D3D12_COMMAND_LIST_TYPE const list_type)
        : allocator(allocator), pipeline_cache(pipeline_cache), descriptor_allocator(descriptor_allocator),
          command_list(command_list), list_type(list_type)
    {
    }

    auto DX12CommandBuffer::reset() -> void
    {
        THROW_IF_FAILED(command_list->Reset(allocator, nullptr));

        if (descriptor_allocator)
        {
            auto descriptors = std::array<ID3D12DescriptorHeap*, 2>{
                descriptor_allocator->get_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
                descriptor_allocator->get_heap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)};
            command_list->SetDescriptorHeaps(static_cast<uint32_t>(descriptors.size()), descriptors.data());
        }

        is_root_signature_binded = false;
        current_shader = nullptr;
    }

    auto DX12CommandBuffer::close() -> void
    {
        THROW_IF_FAILED(command_list->Close());
    }

    auto DX12CommandBuffer::set_graphics_pipeline_options(core::ref_ptr<Shader> shader,
                                                          RasterizerStageInfo const& rasterizer,
                                                          BlendColorInfo const& blend_color,
                                                          std::optional<DepthStencilStageInfo> const depth_stencil)
        -> void
    {
        auto pipeline = pipeline_cache->get(static_cast<DX12Shader&>(&shader), rasterizer, blend_color, depth_stencil,
                                            render_target_formats, depth_stencil_format);

        if (!is_root_signature_binded)
        {
            command_list->SetGraphicsRootSignature(pipeline->get_root_signature());
            is_root_signature_binded = true;
        }

        command_list->SetPipelineState(pipeline->get_pipeline_state());
        command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        current_shader = shader;
    }

    auto DX12CommandBuffer::bind_descriptor(std::string_view const binding,
                                            std::variant<BufferBindData, TextureBindData> data) -> void
    {
        uint32_t const index = current_shader->get_bindings().at(std::string(binding));
        auto visitor = make_visitor(
            [&](BufferBindData& data) {
                bindings[index] = static_cast<DX12Buffer*>(data.resource.get())->get_descriptor(data.usage).offset;

                D3D12_RESOURCE_STATES state;
                switch (data.usage)
                {
                    case BufferUsage::ConstantBuffer: {
                        state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
                        break;
                    }
                    case BufferUsage::ShaderResource: {
                        state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
                        break;
                    }
                    case BufferUsage::UnorderedAccess: {
                        state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                        break;
                    }
                }

                {
                    BarrierProxy barrier_proxy(command_list.get());
                    barrier_proxy.add_transition(static_cast<DX12Buffer&>(&data.resource), state);
                }
            },
            [&](TextureBindData& data) {
                bindings[index] = static_cast<DX12Texture*>(data.resource.get())->get_descriptor(data.usage).offset;

                D3D12_RESOURCE_STATES state;
                switch (data.usage)
                {
                    case TextureUsage::ShaderResource: {
                        state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                        break;
                    }
                    case TextureUsage::UnorderedAccess: {
                        state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                        break;
                    }
                }

                {
                    BarrierProxy barrier_proxy(command_list.get());
                    barrier_proxy.add_transition(static_cast<DX12Texture&>(&data.resource), state);
                }
            });
        std::visit(visitor, data);
    }

    auto DX12CommandBuffer::begin_render_pass(std::span<RenderPassColorInfo> const colors,
                                              std::optional<RenderPassDepthStencilInfo> depth_stencil) -> void
    {
        render_target_formats.fill(DXGI_FORMAT_UNKNOWN);
        depth_stencil_format = DXGI_FORMAT_UNKNOWN;
        render_target_textures.fill(nullptr);
        depth_stencil_texture = nullptr;

        auto render_pass_render_targets =
            std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>{};

        BarrierProxy barrier_proxy(command_list.get());

        for (uint32_t const i : std::views::iota(0u, colors.size()))
        {
            auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
            begin.Type = render_pass_load_to_d3d12(colors[i].load_op);

            begin.Clear.ClearValue.Format = texture_format_to_dxgi(colors[i].texture->get_format());
            begin.Clear.ClearValue.Color[0] = colors[i].clear_color.r;
            begin.Clear.ClearValue.Color[1] = colors[i].clear_color.g;
            begin.Clear.ClearValue.Color[2] = colors[i].clear_color.b;
            begin.Clear.ClearValue.Color[3] = colors[i].clear_color.a;

            auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
            end.Type = render_pass_store_to_d3d12(colors[i].store_op);

            render_pass_render_targets[i].BeginningAccess = begin;
            render_pass_render_targets[i].EndingAccess = end;
            render_pass_render_targets[i].cpuDescriptor = static_cast<DX12Texture*>(colors[i].texture.get())
                                                              ->get_descriptor(TextureUsage::RenderTarget)
                                                              .cpu_handle();

            render_target_formats[i] = begin.Clear.ClearValue.Format;

            barrier_proxy.add_transition(static_cast<DX12Texture&>(&colors[i].texture),
                                         D3D12_RESOURCE_STATE_RENDER_TARGET);

            render_target_textures[i] = static_cast<DX12Texture*>(colors[i].texture.get());
        }

        if (depth_stencil.has_value())
        {
            auto value = depth_stencil.value();
            depth_stencil_format = texture_format_to_dxgi(value.texture->get_format());

            auto render_pass_depth_stencil = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC{};
            {
                auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
                begin.Type = render_pass_load_to_d3d12(value.depth_load_op);
                begin.Clear.ClearValue.Format = depth_stencil_format;
                begin.Clear.ClearValue.DepthStencil.Depth = value.clear_depth;

                auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
                end.Type = render_pass_store_to_d3d12(value.depth_store_op);

                render_pass_depth_stencil.DepthBeginningAccess = begin;
                render_pass_depth_stencil.DepthEndingAccess = end;
            }

            {
                auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
                begin.Type = render_pass_load_to_d3d12(value.stencil_load_op);
                begin.Clear.ClearValue.Format = depth_stencil_format;
                begin.Clear.ClearValue.DepthStencil.Depth = value.clear_depth;

                auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
                end.Type = render_pass_store_to_d3d12(value.stencil_store_op);

                render_pass_depth_stencil.StencilBeginningAccess = begin;
                render_pass_depth_stencil.StencilEndingAccess = end;
            }
            render_pass_depth_stencil.cpuDescriptor =
                static_cast<DX12Texture*>(value.texture.get())->get_descriptor(TextureUsage::DepthStencil).cpu_handle();

            depth_stencil_texture = static_cast<DX12Texture*>(value.texture.get());

            barrier_proxy.add_transition(static_cast<DX12Texture&>(&value.texture), D3D12_RESOURCE_STATE_DEPTH_WRITE);

            barrier_proxy.~BarrierProxy();

            command_list->BeginRenderPass(static_cast<uint32_t>(colors.size()), render_pass_render_targets.data(),
                                          &render_pass_depth_stencil, D3D12_RENDER_PASS_FLAG_NONE);
        }
        else
        {
            barrier_proxy.~BarrierProxy();

            command_list->BeginRenderPass(static_cast<uint32_t>(colors.size()), render_pass_render_targets.data(),
                                          nullptr, D3D12_RENDER_PASS_FLAG_NONE);
        }
    }

    auto DX12CommandBuffer::end_render_pass() -> void
    {
        command_list->EndRenderPass();

        {
            BarrierProxy barrier_proxy(command_list.get());
            for (auto texture : render_target_textures)
            {
                if (!texture)
                {
                    continue;
                }
                barrier_proxy.add_transition(*texture, D3D12_RESOURCE_STATE_COMMON);
            }

            if (depth_stencil_texture)
            {
                barrier_proxy.add_transition(*depth_stencil_texture, D3D12_RESOURCE_STATE_DEPTH_READ);
            }
        }
    }

    auto DX12CommandBuffer::bind_vertex_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
        -> void
    {
        {
            BarrierProxy barrier_proxy(command_list.get());
            barrier_proxy.add_transition(static_cast<DX12Buffer&>(&buffer),
                                         D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        }

        auto d3d12_vertex_buffer_view = D3D12_VERTEX_BUFFER_VIEW{};
        d3d12_vertex_buffer_view.BufferLocation =
            static_cast<DX12Buffer*>(buffer.get())->get_resource()->GetGPUVirtualAddress() + offset;
        d3d12_vertex_buffer_view.SizeInBytes = static_cast<uint32_t>(size);
        d3d12_vertex_buffer_view.StrideInBytes = current_shader->get_inputs_size_per_vertex();

        command_list->IASetVertexBuffers(0, 1, &d3d12_vertex_buffer_view);
    }

    auto DX12CommandBuffer::bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                              IndexFormat const format) -> void
    {
        {
            BarrierProxy barrier_proxy(command_list.get());
            barrier_proxy.add_transition(static_cast<DX12Buffer&>(&buffer), D3D12_RESOURCE_STATE_INDEX_BUFFER);
        }

        auto d3d12_index_buffer_view = D3D12_INDEX_BUFFER_VIEW{};
        d3d12_index_buffer_view.BufferLocation =
            static_cast<DX12Buffer*>(buffer.get())->get_resource()->GetGPUVirtualAddress() + offset;
        switch (format)
        {
            case IndexFormat::Uint32: {
                d3d12_index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
                break;
            }
            case IndexFormat::Uint16: {
                d3d12_index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
                break;
            }
        }
        d3d12_index_buffer_view.SizeInBytes = size;

        command_list->IASetIndexBuffer(&d3d12_index_buffer_view);
    }

    auto DX12CommandBuffer::draw_indexed(uint32_t const index_count, uint32_t const instance_count,
                                         uint32_t const instance_offset) -> void
    {
        command_list->SetGraphicsRoot32BitConstants(0, static_cast<uint32_t>(bindings.size()), bindings.data(), 0);

        command_list->DrawIndexedInstanced(index_count, instance_count, 0, 0, instance_offset);
    }

    auto DX12CommandBuffer::draw(uint32_t const vertex_count, uint32_t const instance_count,
                                 uint32_t const instance_offset) -> void
    {
        command_list->SetGraphicsRoot32BitConstants(0, static_cast<uint32_t>(bindings.size()), bindings.data(), 0);

        command_list->DrawInstanced(vertex_count, instance_count, 0, instance_offset);
    }

    auto DX12CommandBuffer::copy_buffer(core::ref_ptr<Buffer> dst, uint64_t const dst_offset, core::ref_ptr<Buffer> src,
                                        uint64_t const src_offset, size_t const size) -> void
    {
        {
            BarrierProxy barrier_proxy(command_list.get());
            barrier_proxy.add_transition(static_cast<DX12Buffer&>(&dst), D3D12_RESOURCE_STATE_COPY_DEST);
        }

        command_list->CopyBufferRegion(static_cast<DX12Buffer*>(dst.get())->get_resource(), dst_offset,
                                       static_cast<DX12Buffer*>(src.get())->get_resource(), src_offset, size);

        {
            BarrierProxy barrier_proxy(command_list.get());
            barrier_proxy.add_transition(static_cast<DX12Buffer&>(&dst), D3D12_RESOURCE_STATE_COMMON);
        }
    }

    auto DX12CommandBuffer::copy_buffer(core::ref_ptr<Texture> dst, core::ref_ptr<Buffer> src,
                                        std::span<TextureCopyRegion const> const regions) -> void
    {
        {
            BarrierProxy barrier_proxy(command_list.get());
            barrier_proxy.add_transition(static_cast<DX12Texture&>(&dst), D3D12_RESOURCE_STATE_COPY_DEST);
        }

        for (uint32_t const i : std::views::iota(0u, regions.size()))
        {
            auto d3d12_subresource_footprint = D3D12_PLACED_SUBRESOURCE_FOOTPRINT{};
            d3d12_subresource_footprint.Footprint.Format =
                static_cast<DX12Texture*>(dst.get())->get_resource()->GetDesc().Format;
            d3d12_subresource_footprint.Footprint.Width = regions[i].mip_width;
            d3d12_subresource_footprint.Footprint.Height = regions[i].mip_height;
            d3d12_subresource_footprint.Footprint.Depth = regions[i].mip_depth;
            d3d12_subresource_footprint.Footprint.RowPitch = regions[i].row_pitch;
            d3d12_subresource_footprint.Offset = regions[i].offset;

            auto d3d12_source_location = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12_source_location.pResource = static_cast<DX12Buffer*>(src.get())->get_resource();
            d3d12_source_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            d3d12_source_location.PlacedFootprint = d3d12_subresource_footprint;

            auto d3d12_dest_location = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12_dest_location.pResource = static_cast<DX12Texture*>(dst.get())->get_resource();
            d3d12_dest_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            d3d12_dest_location.SubresourceIndex = i;

            command_list->CopyTextureRegion(&d3d12_dest_location, 0, 0, 0, &d3d12_source_location, nullptr);
        }

        {
            BarrierProxy barrier_proxy(command_list.get());
            barrier_proxy.add_transition(static_cast<DX12Texture&>(&dst), D3D12_RESOURCE_STATE_COMMON);
        }
    }

    auto DX12CommandBuffer::set_viewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
        -> void
    {
        auto d3d12_viewport = D3D12_VIEWPORT{};
        d3d12_viewport.TopLeftX = static_cast<float>(x);
        d3d12_viewport.TopLeftY = static_cast<float>(y);
        d3d12_viewport.Width = static_cast<float>(width);
        d3d12_viewport.Height = static_cast<float>(height);
        d3d12_viewport.MinDepth = D3D12_MIN_DEPTH;
        d3d12_viewport.MaxDepth = D3D12_MAX_DEPTH;

        command_list->RSSetViewports(1, &d3d12_viewport);
    }

    auto DX12CommandBuffer::set_scissor(int32_t const left, int32_t const top, int32_t const right,
                                        int32_t const bottom) -> void
    {
        auto d3d12_rect = D3D12_RECT{};
        d3d12_rect.top = static_cast<LONG>(top);
        d3d12_rect.bottom = static_cast<LONG>(bottom);
        d3d12_rect.left = static_cast<LONG>(left);
        d3d12_rect.right = static_cast<LONG>(right);

        command_list->RSSetScissorRects(1, &d3d12_rect);
    }
} // namespace ionengine::renderer::rhi