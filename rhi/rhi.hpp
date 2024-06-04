// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "math/color.hpp"

namespace ionengine::platform
{
    class Window;
}

namespace ionengine::rhi
{
    enum class BufferUsage
    {
        Vertex = 1 << 0,
        Index = 1 << 1,
        ConstantBuffer = 1 << 2,
        ShaderResource = 1 << 3,
        UnorderedAccess = 1 << 4,
        MapWrite = 1 << 5,
        MapRead = 1 << 6,
        CopySrc = 1 << 7,
        CopyDst = 1 << 8
    };

    DECLARE_ENUM_CLASS_BIT_FLAG(BufferUsage)

    class Buffer : public core::ref_counted_object
    {
      public:
        virtual auto get_size() -> size_t = 0;

        virtual auto get_flags() -> BufferUsageFlags = 0;

        virtual auto map_memory() -> uint8_t* = 0;

        virtual auto unmap_memory() -> void = 0;

        virtual auto get_descriptor_offset(BufferUsage const usage) const -> uint32_t = 0;
    };

    enum class TextureFormat
    {
        Unknown,
        BGRA8_UNORM,
        BGR8_UNORM,
        RGBA8_UNORM,
        R8_UNORM,
        BC1,
        BC3,
        BC4,
        BC5,
        D32_FLOAT,
        RGBA16_FLOAT
    };

    enum class TextureDimension
    {
        _1D,
        _2D,
        _2DArray,
        _3D,
        Cube,
        CubeArray
    };

    enum class TextureUsage
    {
        ShaderResource = 1 << 0,
        RenderTarget = 1 << 1,
        DepthStencil = 1 << 2,
        UnorderedAccess = 1 << 3,
        CopySrc = 1 << 4,
        CopyDst = 1 << 5
    };

    DECLARE_ENUM_CLASS_BIT_FLAG(TextureUsage)

    class Texture : public core::ref_counted_object
    {
      public:
        virtual auto get_width() const -> uint32_t = 0;

        virtual auto get_height() const -> uint32_t = 0;

        virtual auto get_depth() const -> uint32_t = 0;

        virtual auto get_mip_levels() const -> uint32_t = 0;

        virtual auto get_format() const -> TextureFormat = 0;

        virtual auto get_flags() const -> TextureUsageFlags = 0;

        virtual auto get_descriptor_offset(TextureUsage const usage) const -> uint32_t = 0;
    };

    class Shader : public core::ref_counted_object
    {
    };

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

    enum class ResourceState
    {
        Common,
        RenderTarget,
        DepthRead,
        DepthWrite,
        NonPixelShaderRead,
        PixelShaderRead,
        AllShaderRead,
        UnorderedAccess,
        CopyDst,
        CopySrc
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

    enum class VertexFormat
    {
        Float4x4,
        Float3x3,
        Float2x2,
        Float4,
        Float3,
        Float2,
        Float,
        Uint,
        Bool
    };

    struct VertexDeclarationInfo
    {
        std::string semantic;
        uint32_t index;
        VertexFormat format;
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

    class FutureImpl
    {
      public:
        virtual auto get_result() const -> bool = 0;

        virtual auto wait() -> void = 0;
    };

    template <typename Type>
    class Future
    {
        template <typename Derived>
        friend class Future;

      public:
        Future() : ptr(nullptr), impl(nullptr)
        {
        }

        Future(core::ref_ptr<Type> ptr, std::unique_ptr<FutureImpl>&& impl) : ptr(ptr), impl(std::move(impl))
        {
        }

        Future(Future&& other) : ptr(std::move(other.ptr)), impl(std::move(other.impl))
        {
        }

        template <typename Derived>
        Future(Future<Derived> other) : ptr(other.ptr), impl(std::move(other.impl))
        {
        }

        Future(Future const&) = delete;

        auto operator=(Future&& other) -> Future&
        {
            ptr = std::move(other.ptr);
            impl = std::move(other.impl);
            return *this;
        }

        template <typename Derived>
        auto operator=(Future<Derived> other) -> Future&
        {
            ptr = other.ptr;
            impl = std::move(other.impl);
            return *this;
        }

        auto operator=(Future const&) -> Future& = delete;

        auto get_result() const -> bool
        {
            return impl->get_result();
        }

        auto wait() -> void
        {
            impl->wait();
        }

        auto get() -> core::ref_ptr<Type>
        {
            if (!get_result())
            {
                wait();
            }
            return ptr;
        }

      private:
        core::ref_ptr<Type> ptr;
        std::unique_ptr<FutureImpl> impl;
    };

    class Query : public core::ref_counted_object
    {
    };

    class GraphicsContext : public core::ref_counted_object
    {
      public:
        virtual auto reset() -> void = 0;

        virtual auto set_graphics_pipeline_options(
            core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blend_color,
            std::optional<DepthStencilStageInfo> const depth_stencil) -> void = 0;

        virtual auto bind_descriptor(uint32_t const index, uint32_t const descriptor) -> void = 0;

        virtual auto begin_render_pass(std::span<RenderPassColorInfo> const colors,
                                       std::optional<RenderPassDepthStencilInfo> depth_stencil) -> void = 0;

        virtual auto end_render_pass() -> void = 0;

        virtual auto bind_vertex_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset,
                                        size_t const size) -> void = 0;

        virtual auto bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                       IndexFormat const format) -> void = 0;

        virtual auto draw_indexed(uint32_t const index_count, uint32_t const instance_count) -> void = 0;

        virtual auto draw(uint32_t const vertex_count, uint32_t const instance_count) -> void = 0;

        virtual auto set_viewport(int32_t const x, int32_t const y, uint32_t const width,
                                  uint32_t const height) -> void = 0;

        virtual auto set_scissor(int32_t const left, int32_t const top, int32_t const right,
                                 int32_t const bottom) -> void = 0;

        virtual auto barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst,
                             ResourceState const before, ResourceState const after) -> void = 0;

        virtual auto execute() -> Future<Query> = 0;
    };

    class CopyContext : public core::ref_counted_object
    {
      public:
        virtual auto reset() -> void = 0;

        virtual auto write_buffer(core::ref_ptr<Buffer> dst, std::span<uint8_t const> const data) -> Future<Buffer> = 0;

        virtual auto write_texture(core::ref_ptr<Texture> dst,
                                   std::vector<std::span<uint8_t const>> const& data) -> Future<Texture> = 0;

        virtual auto read_buffer(core::ref_ptr<Buffer> dst, std::vector<uint8_t>& data) -> void = 0;

        virtual auto read_texture(core::ref_ptr<Texture> dst, std::vector<std::vector<uint8_t>>& data) -> void = 0;

        virtual auto barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst,
                             ResourceState const before, ResourceState const after) -> void = 0;

        virtual auto execute() -> Future<Query> = 0;
    };

    class Device : public core::ref_counted_object
    {
      public:
        static auto create(platform::Window* window) -> core::ref_ptr<Device>;

        virtual auto createShader(std::span<VertexDeclarationInfo const> const vertexDeclarations,
                                  std::span<uint8_t const> const vertexShader,
                                  std::span<uint8_t const> const pixelShader) -> core::ref_ptr<Shader> = 0;

        virtual auto createShader(std::span<uint8_t const> const computeShader) -> core::ref_ptr<Shader> = 0;

        virtual auto create_texture(uint32_t const width, uint32_t const height, uint32_t const depth,
                                    uint32_t const mip_levels, TextureFormat const format,
                                    TextureDimension const dimension,
                                    TextureUsageFlags const flags) -> core::ref_ptr<Texture> = 0;

        virtual auto create_buffer(size_t const size, size_t const element_stride,
                                   BufferUsageFlags const flags) -> core::ref_ptr<Buffer> = 0;

        virtual auto create_graphics_context() -> core::ref_ptr<GraphicsContext> = 0;

        virtual auto create_copy_context() -> core::ref_ptr<CopyContext> = 0;

        virtual auto request_back_buffer() -> core::ref_ptr<Texture> = 0;

        virtual auto present_back_buffer() -> void = 0;

        virtual auto getBackendType() const -> std::string_view = 0;
    };
} // namespace ionengine::rhi