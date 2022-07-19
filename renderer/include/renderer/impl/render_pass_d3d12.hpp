// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/impl/d3d12.hpp>
#include <renderer/render_pass.hpp>

namespace ionengine::renderer {

class Texture_D3D12;

///
/// @private
///
class RenderPass_D3D12 final : public RenderPass {
 public:
    virtual void draw(CommandList& command_list, uint32_t const vertex_count,
                      uint32_t const instance_count,
                      uint32_t const vertex_offset) override;

    virtual void draw_indexed(CommandList& command_list,
                              uint32_t const index_count,
                              uint32_t const instance_count,
                              uint32_t const instance_offset) override;

    virtual void bind_vertex_buffer(CommandList& command_list,
                                    uint32_t const slot, Buffer& buffer) override;

    virtual void bind_index_buffer(CommandList& command_list,
                                   Buffer& buffer) override;

    static core::Expected<std::unique_ptr<RenderPass>, std::string> create(
        Device_D3D12& device, std::span<Texture_D3D12* const> const colors,
        std::span<RenderPassColorDesc const> const color_descs,
        Texture_D3D12* const depth_stencil,
        std::optional<RenderPassDepthStencilDesc> const
            depth_stencil_desc) noexcept;

 private:
    std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> _render_targets;
    std::optional<D3D12_RENDER_PASS_DEPTH_STENCIL_DESC> _depth_stencil;
};

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE to_render_pass_beginning_access(
    RenderPassLoadOp const load_op);

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE to_render_pass_ending_access(
    RenderPassStoreOp const store_op);

}  // namespace ionengine::renderer