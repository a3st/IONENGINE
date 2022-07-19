// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.h>

#include <math/color.hpp>

namespace ionengine::renderer {

class Device;
class Pipeline;
class RenderPass;
class Buffer;
class Texture;

///
/// Texture copy region description
///
struct TextureCopyRegionDesc {
    uint32_t mip_index;
    uint32_t row_pitch;
    uint64_t offset;
};

///
/// Command list types
///
enum class CommandListType { Direct, Copy, Compute };

DECLARE_ENUM_CLASS_BIT_FLAG(CommandListType)

class CommandList {
 public:
    virtual void dispatch(uint32_t const thread_group_x,
                          uint32_t const thread_group_y,
                          uint32_t const thread_group_z) = 0;

    virtual void begin() = 0;

    virtual void close() = 0;

    virtual void begin_render_pass(
        RenderPass& render_pass,
        std::span<math::Color const> const clear_colors,
        float const clear_depth, uint8_t const clear_stencil) = 0;

    virtual void end_render_pass() = 0;

    virtual void set_viewport(int32_t const x, int32_t const y,
                              uint32_t const width, uint32_t const height) = 0;

    virtual void set_scissor(int32_t const left, int32_t const top,
                             int32_t const right, int32_t const bottom) = 0;

    virtual void bind_pipeline(Pipeline& pipeline) = 0;

    virtual void copy_buffer_region(Buffer& dest, uint64_t const dest_offset,
                                    Buffer& source,
                                    uint64_t const source_offset,
                                    size_t const size) = 0;

    virtual void copy_texture_region(
        Texture& dest, Buffer& source,
        std::span<TextureCopyRegionDesc const> const regions) = 0;

    static core::Expected<std::unique_ptr<CommandList>, std::string> create(
        Device& device, CommandListType const list_type,
        bool const bundled) noexcept;
};

}  // namespace ionengine::renderer