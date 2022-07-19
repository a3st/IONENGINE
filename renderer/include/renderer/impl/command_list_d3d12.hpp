// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/command_list.hpp>
#include <renderer/impl/d3d12.hpp>

namespace ionengine::renderer {
    
class Device_D3D12;

///
/// @private
///
class CommandList_D3D12 final : public CommandList {
 public:
    virtual void dispatch(uint32_t const thread_group_x,
                          uint32_t const thread_group_y,
                          uint32_t const thread_group_z) override;

    virtual void begin() override;

    virtual void close() override;

    virtual void begin_render_pass(
        RenderPass& render_pass,
        std::span<math::Color const> const clear_colors,
        float const clear_depth, uint8_t const clear_stencil) override;

    virtual void end_render_pass() override;

    virtual void set_viewport(int32_t const x, int32_t const y,
                              uint32_t const width, uint32_t const height) override;

    virtual void set_scissor(int32_t const left, int32_t const top,
                             int32_t const right, int32_t const bottom) override;

    virtual void bind_pipeline(Pipeline& pipeline) override;

    virtual void copy_buffer_region(Buffer& dest, uint64_t const dest_offset,
                                    Buffer& source,
                                    uint64_t const source_offset,
                                    size_t const size) override;

    virtual void copy_texture_region(
        Texture& dest, Buffer& source,
        std::span<TextureCopyRegionDesc const> const regions) override;

    static core::Expected<std::unique_ptr<CommandList>, std::string> create(
        Device_D3D12& device, CommandListType const list_type,
        bool const bundled) noexcept;

 private:
    WRL::ComPtr<ID3D12GraphicsCommandList4> _command_list;
};

///
/// @private
///
D3D12_COMMAND_LIST_TYPE constexpr to_command_list(
    CommandListType const list_type);

}  // namespace ionengine::renderer