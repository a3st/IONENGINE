// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>

#include <renderer/impl/device_d3d12.hpp>
#include <renderer/impl/render_pass_d3d12.hpp>
#include <renderer/impl/texture_d3d12.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE
ionengine::renderer::to_render_pass_beginning_access(
    RenderPassLoadOp const load_op) {
    switch (load_op) {
        case RenderPassLoadOp::Load:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
        case RenderPassLoadOp::Clear:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        case RenderPassLoadOp::DontCare:
            return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        default:
            throw std::invalid_argument("Invalid argument");
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE
ionengine::renderer::to_render_pass_ending_access(
    RenderPassStoreOp const store_op) {
    switch (store_op) {
        case RenderPassStoreOp::Store:
            return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        case RenderPassStoreOp::DontCare:
            return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        default:
            throw std::invalid_argument("Invalid argument");
    }
}

void RenderPass_D3D12::draw(CommandList& command_list, uint32_t const vertex_count,
          uint32_t const instance_count, uint32_t const vertex_offset) {}

void RenderPass_D3D12::draw_indexed(CommandList& command_list, uint32_t const index_count,
                  uint32_t const instance_count,
                  uint32_t const instance_offset) {}

void RenderPass_D3D12::bind_vertex_buffer(CommandList& command_list, uint32_t const slot,
                        Buffer& buffer) {}

void RenderPass_D3D12::bind_index_buffer(CommandList& command_list, Buffer& buffer) {}

core::Expected<std::unique_ptr<RenderPass>, std::string>
RenderPass_D3D12::create(Device_D3D12& device,
                         std::span<Texture_D3D12* const> const colors,
                         std::span<RenderPassColorDesc const> const color_descs,
                         Texture_D3D12* const depth_stencil,
                         std::optional<RenderPassDepthStencilDesc> const
                             depth_stencil_desc) noexcept {
    auto render_pass = std::make_unique<RenderPass_D3D12>();

    // Initialize class members
    render_pass->_render_targets.resize(colors.size());

    for (size_t i = 0; i < render_pass->_render_targets.size(); ++i) {
        D3D12_RENDER_PASS_BEGINNING_ACCESS begin = {
            .Type = to_render_pass_beginning_access(color_descs[i].load_op)};
        begin.Clear.ClearValue.Format = colors[i]->_resource->GetDesc().Format;

        D3D12_RENDER_PASS_ENDING_ACCESS end = {
            .Type = to_render_pass_ending_access(color_descs[i].store_op)};

        render_pass->_render_targets[i].BeginningAccess = begin;
        render_pass->_render_targets[i].EndingAccess = end;
        render_pass->_render_targets[i].cpuDescriptor =
            colors[i]->_descriptor_allocs[0]->CPUHandle();

        std::cout << colors[i]->_resource->GetDesc().Format << std::endl;
    }

    if (depth_stencil) {
        D3D12_RENDER_PASS_BEGINNING_ACCESS depth_begin = {
            .Type = to_render_pass_beginning_access(
                depth_stencil_desc.value().depth_load_op)};
        depth_begin.Clear.ClearValue.Format = DXGI_FORMAT_D32_FLOAT;

        D3D12_RENDER_PASS_ENDING_ACCESS depth_end = {
            .Type = to_render_pass_ending_access(
                depth_stencil_desc.value().depth_store_op)};

        D3D12_RENDER_PASS_BEGINNING_ACCESS stencil_begin = {
            .Type = to_render_pass_beginning_access(
                depth_stencil_desc.value().stencil_load_op)};
        depth_begin.Clear.ClearValue.Format = DXGI_FORMAT_D32_FLOAT;

        D3D12_RENDER_PASS_ENDING_ACCESS stencil_end = {
            .Type = to_render_pass_ending_access(
                depth_stencil_desc.value().stencil_store_op)};

        render_pass->_depth_stencil = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC{
            .cpuDescriptor = depth_stencil->_descriptor_allocs[0]->CPUHandle(),
            .DepthBeginningAccess = depth_begin,
            .StencilBeginningAccess = stencil_begin,
            .DepthEndingAccess = depth_end,
            .StencilEndingAccess = stencil_end};
    }

    return core::make_expected<std::unique_ptr<RenderPass>, std::string>(
        std::move(render_pass));
}

core::Expected<std::unique_ptr<RenderPass>, std::string> RenderPass::create(
    Device& device, std::span<Texture* const> const colors,
    std::span<RenderPassColorDesc const> const color_descs,
    Texture* const depth_stencil,
    std::optional<RenderPassDepthStencilDesc> const
        depth_stencil_desc) noexcept {
    return RenderPass_D3D12::create(
        static_cast<Device_D3D12&>(device),
        std::span<Texture_D3D12* const>(
            reinterpret_cast<Texture_D3D12* const*>(colors.data()),
            colors.size()),
        color_descs, static_cast<Texture_D3D12*>(depth_stencil),
        depth_stencil_desc);
}