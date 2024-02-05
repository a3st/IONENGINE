// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "core/ref_ptr.hpp"
#include "math/color.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace ionengine::renderer::rhi
{
    enum class RenderPassLoadOp
    {
        Load,
        Clear,
        DontCare
    };

    enum class RenderPassStoreOp
    {
        Store,
        DontCare
    };

    enum class IndexFormat
    {
        Uint16,
        Uint32
    };

    enum class FillMode
    {
        Wireframe,
        Solid
    };

    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class CompareOp
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    enum class Blend
    {
        Zero,
        One,
        SrcAlpha,
        InvSrcAlpha,
        BlendFactor
    };

    enum class BlendOp
    {
        Add,
        Subtract,
        RevSubtract,
        Min,
        Max
    };

    enum class CommandBufferType
    {
        Graphics,
        Copy,
        Compute
    };

    struct RenderPassColorInfo
    {
        core::ref_ptr<Texture> texture;
        RenderPassLoadOp load_op;
        RenderPassStoreOp store_op;
        math::Color clear_color;
    };

    struct RenderPassDepthStencilInfo
    {
        core::ref_ptr<Texture> texture;
        RenderPassLoadOp depth_load_op;
        RenderPassStoreOp depth_store_op;
        RenderPassLoadOp stencil_load_op;
        RenderPassStoreOp stencil_store_op;
        float clear_depth;
        uint8_t clear_stencil;
    };

    struct DepthStencilStageInfo
    {
        CompareOp depth_func;
        bool depth_write;
        bool stencil_write;

        static auto Default() -> DepthStencilStageInfo const&
        {
            static const DepthStencilStageInfo instance = {
                .depth_func = CompareOp::Never, .depth_write = false, .stencil_write = false};
            return instance;
        }
    };

    struct BlendColorInfo
    {
        bool blend_enable;
        Blend blend_src;
        Blend blend_dst;
        BlendOp blend_op;
        Blend blend_src_alpha;
        Blend blend_dst_alpha;
        BlendOp blend_op_alpha;

        static auto Opaque() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance = {.blend_enable = false,
                                                    .blend_src = Blend::Zero,
                                                    .blend_dst = Blend::Zero,
                                                    .blend_op = BlendOp::Add,
                                                    .blend_src_alpha = Blend::Zero,
                                                    .blend_dst_alpha = Blend::Zero,
                                                    .blend_op_alpha = BlendOp::Add};
            return instance;
        }

        static auto Add() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance = {.blend_enable = true,
                                                    .blend_src = Blend::One,
                                                    .blend_dst = Blend::One,
                                                    .blend_op = BlendOp::Add,
                                                    .blend_src_alpha = Blend::One,
                                                    .blend_dst_alpha = Blend::Zero,
                                                    .blend_op_alpha = BlendOp::Add};
            return instance;
        }

        static auto Mixed() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance = {.blend_enable = true,
                                                    .blend_src = Blend::One,
                                                    .blend_dst = Blend::InvSrcAlpha,
                                                    .blend_op = BlendOp::Add,
                                                    .blend_src_alpha = Blend::One,
                                                    .blend_dst_alpha = Blend::Zero,
                                                    .blend_op_alpha = BlendOp::Add};
            return instance;
        }

        static auto AlphaBlend() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance = {.blend_enable = true,
                                                    .blend_src = Blend::SrcAlpha,
                                                    .blend_dst = Blend::InvSrcAlpha,
                                                    .blend_op = BlendOp::Add,
                                                    .blend_src_alpha = Blend::One,
                                                    .blend_dst_alpha = Blend::Zero,
                                                    .blend_op_alpha = BlendOp::Add};
            return instance;
        }
    };

    struct RasterizerStageInfo
    {
        FillMode fill_mode;
        CullMode cull_mode;
    };

    struct BufferBindData
    {
        core::ref_ptr<Buffer> resource;
        BufferUsage usage;
    };

    struct TextureBindData
    {
        core::ref_ptr<Texture> resource;
        TextureUsage usage;
    };

    struct TextureCopyRegion
    {
        uint32_t mip_width;
        uint32_t mip_height;
        uint32_t mip_depth;
        uint32_t row_pitch;
        uint64_t offset;
    };

    class CommandBuffer : public core::ref_counted_object
    {
      public:
        virtual auto close() -> void = 0;

        virtual auto set_graphics_pipeline_options(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                                   BlendColorInfo const& blend_color,
                                                   std::optional<DepthStencilStageInfo> const depth_stencil)
            -> void = 0;

        virtual auto bind_descriptor(std::string_view const binding,
                                     std::variant<BufferBindData, TextureBindData> data) -> void = 0;

        virtual auto begin_render_pass(std::span<RenderPassColorInfo> const colors,
                                       std::optional<RenderPassDepthStencilInfo> depth_stencil) -> void = 0;

        virtual auto end_render_pass() -> void = 0;

        virtual auto bind_vertex_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
            -> void = 0;

        virtual auto bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                       IndexFormat const format) -> void = 0;

        virtual auto draw_indexed(uint32_t const index_count, uint32_t const instance_count,
                                  const uint32_t instance_offset) -> void = 0;

        virtual auto draw(uint32_t const vertex_count, uint32_t const instance_count, uint32_t const instance_offset)
            -> void = 0;

        virtual auto copy_buffer(core::ref_ptr<Buffer> dst, uint64_t const dst_offset, core::ref_ptr<Buffer> src,
                                 uint64_t const src_offset, size_t const size) -> void = 0;

        virtual auto copy_buffer(core::ref_ptr<Texture> dst, core::ref_ptr<Buffer> src,
                                 std::span<TextureCopyRegion const> const regions) -> void = 0;

        virtual auto set_viewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
            -> void = 0;

        virtual auto set_scissor(int32_t const left, int32_t const top, int32_t const right, int32_t const bottom)
            -> void = 0;
    };
} // namespace ionengine::renderer::rhi