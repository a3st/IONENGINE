// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/bit_utils.hpp"
#include "core/color.hpp"
#include "core/ref_ptr.hpp"

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
        CopySource = 1 << 7,
        CopyDest = 1 << 8
    };

    DECLARE_ENUM_CLASS_BIT_FLAG(BufferUsage)

    class Buffer : public core::ref_counted_object
    {
      public:
        virtual ~Buffer() = default;

        virtual auto getSize() const -> size_t = 0;

        virtual auto getFlags() const -> BufferUsageFlags = 0;

        virtual auto getDescriptorOffset(BufferUsage const usage) const -> uint32_t = 0;
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
        CopySource = 1 << 4,
        CopyDest = 1 << 5
    };

    DECLARE_ENUM_CLASS_BIT_FLAG(TextureUsage)

    class Texture : public core::ref_counted_object
    {
      public:
        virtual ~Texture() = default;

        virtual auto getWidth() const -> uint32_t = 0;

        virtual auto getHeight() const -> uint32_t = 0;

        virtual auto getDepth() const -> uint32_t = 0;

        virtual auto getMipLevels() const -> uint32_t = 0;

        virtual auto getFormat() const -> TextureFormat = 0;

        virtual auto getFlags() const -> TextureUsageFlags = 0;

        virtual auto getDescriptorOffset(TextureUsage const usage) const -> uint32_t = 0;
    };

    enum class ShaderType
    {
        Graphics,
        Compute
    };

    class Shader : public core::ref_counted_object
    {
      public:
        virtual ~Shader() = default;

        virtual auto getShaderType() const -> ShaderType = 0;
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
        DepthStencilRead,
        DepthStencilWrite,
        ShaderRead,
        UnorderedAccess,
        CopyDest,
        CopySource
    };

    enum class Filter
    {
        Anisotropic,
        MinMagMipLinear,
        ComparisonMinMagMipLinear
    };

    enum class AddressMode
    {
        Wrap,
        Clamp,
        Mirror
    };

    struct SamplerCreateInfo
    {
        Filter filter;
        AddressMode addressU;
        AddressMode addressV;
        AddressMode addressW;
        CompareOp compareOp;
        uint32_t maxAnisotropy;
    };

    class Sampler : public core::ref_counted_object
    {
      public:
        virtual ~Sampler() = default;

        virtual auto getDescriptorOffset() const -> uint32_t = 0;
    };

    struct BufferCreateInfo
    {
        size_t size;
        uint32_t elementStride;
        BufferUsageFlags flags;
    };

    struct TextureCreateInfo
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        TextureFormat format;
        TextureDimension dimension;
        TextureUsageFlags flags;
    };

    struct SwapchainCreateInfo
    {
        void* window;
        void* instance;
        uint32_t frameCount;
    };

    struct RHICreateInfo
    {
        size_t stagingBufferSize;
        uint32_t numBuffering;

        static auto Default() -> RHICreateInfo const&
        {
            static const RHICreateInfo instance{.stagingBufferSize = 8 * 1024 * 1024, .numBuffering = 1};
            return instance;
        }
    };

    struct RenderPassColorInfo
    {
        core::ref_ptr<Texture> texture;
        RenderPassLoadOp loadOp;
        RenderPassStoreOp storeOp;
        core::Color clearColor;
    };

    struct RenderPassDepthStencilInfo
    {
        core::ref_ptr<Texture> texture;
        RenderPassLoadOp depthLoadOp;
        RenderPassStoreOp depthStoreOp;
        RenderPassLoadOp stencilLoadOp;
        RenderPassStoreOp stencilStoreOp;
        float clearDepth;
        uint8_t clearStencil;
    };

    struct DepthStencilStageInfo
    {
        CompareOp depthFunc;
        bool depthWrite;
        bool stencilWrite;

        auto operator==(DepthStencilStageInfo const& other) const -> bool
        {
            return std::make_tuple(depthFunc, depthWrite, stencilWrite) ==
                   std::make_tuple(other.depthFunc, other.depthWrite, other.stencilWrite);
        }

        static auto Default() -> DepthStencilStageInfo const&
        {
            static const DepthStencilStageInfo instance{
                .depthFunc = CompareOp::Never, .depthWrite = false, .stencilWrite = false};
            return instance;
        }
    };

    struct BlendColorInfo
    {
        bool blendEnable;
        Blend blendSrc;
        Blend blendDst;
        BlendOp blendOp;
        Blend blendSrcAlpha;
        Blend blendDstAlpha;
        BlendOp blendOpAlpha;

        auto operator==(BlendColorInfo const& other) const -> bool
        {
            return std::make_tuple(blendEnable, blendSrc, blendDst, blendOp, blendSrcAlpha, blendDstAlpha,
                                   blendOpAlpha) == std::make_tuple(other.blendEnable, other.blendSrc, other.blendDst,
                                                                    other.blendOp, other.blendSrcAlpha,
                                                                    other.blendDstAlpha, other.blendOpAlpha);
        }

        static auto Opaque() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance{.blendEnable = false,
                                                 .blendSrc = Blend::Zero,
                                                 .blendDst = Blend::Zero,
                                                 .blendOp = BlendOp::Add,
                                                 .blendSrcAlpha = Blend::Zero,
                                                 .blendDstAlpha = Blend::Zero,
                                                 .blendOpAlpha = BlendOp::Add};
            return instance;
        }

        static auto Add() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance{.blendEnable = true,
                                                 .blendSrc = Blend::One,
                                                 .blendDst = Blend::One,
                                                 .blendOp = BlendOp::Add,
                                                 .blendSrcAlpha = Blend::One,
                                                 .blendDstAlpha = Blend::Zero,
                                                 .blendOpAlpha = BlendOp::Add};
            return instance;
        }

        static auto Mixed() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance{.blendEnable = true,
                                                 .blendSrc = Blend::One,
                                                 .blendDst = Blend::InvSrcAlpha,
                                                 .blendOp = BlendOp::Add,
                                                 .blendSrcAlpha = Blend::One,
                                                 .blendDstAlpha = Blend::Zero,
                                                 .blendOpAlpha = BlendOp::Add};
            return instance;
        }

        static auto AlphaBlend() -> BlendColorInfo const&
        {
            static const BlendColorInfo instance{.blendEnable = true,
                                                 .blendSrc = Blend::SrcAlpha,
                                                 .blendDst = Blend::InvSrcAlpha,
                                                 .blendOp = BlendOp::Add,
                                                 .blendSrcAlpha = Blend::One,
                                                 .blendDstAlpha = Blend::Zero,
                                                 .blendOpAlpha = BlendOp::Add};
            return instance;
        }
    };

    enum class VertexFormat
    {
        R32_UINT,
        R32_SINT,
        R32_FLOAT,
        RG32_UINT,
        RG32_SINT,
        RG32_FLOAT,
        RGB32_UINT,
        RGB32_SINT,
        RGB32_FLOAT,
        RGBA32_UINT,
        RGBA32_SINT,
        RGBA32_FLOAT
    };

    auto sizeof_VertexFormat(VertexFormat const format) -> size_t;

    struct VertexDeclarationInfo
    {
        std::string semantic;
        VertexFormat format;
    };

    struct ShaderStageCreateInfo
    {
        std::string entryPoint;
        std::span<uint8_t const> shaderCode;
    };

    struct ShaderCreateInfo
    {
        ShaderType shaderType;
        union {
            struct
            {
                std::span<VertexDeclarationInfo const> vertexDeclarations;
                ShaderStageCreateInfo vertexStage;
                ShaderStageCreateInfo pixelStage;
            } graphics;
            ShaderStageCreateInfo compute;
        };

        ~ShaderCreateInfo()
        {
        }
    };

    struct RasterizerStageInfo
    {
        FillMode fillMode;
        CullMode cullMode;

        auto operator==(RasterizerStageInfo const& other) const -> bool
        {
            return std::make_tuple(fillMode, cullMode) == std::make_tuple(other.fillMode, other.cullMode);
        }
    };

    class IDeviceContext;

    class FutureImpl
    {
      public:
        virtual ~FutureImpl() = default;

        virtual auto getResult() const -> bool = 0;

        virtual auto wait() -> void = 0;

        virtual auto waitOnContext(IDeviceContext* context) -> void = 0;
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

        auto getResult() const -> bool
        {
            if (impl)
            {
                return impl->getResult();
            }
            else
            {
                return true;
            }
        }

        auto wait() const -> void
        {
            if (impl)
            {
                impl->wait();
            }
        }

        auto get() -> core::ref_ptr<Type>
        {
            if (!this->getResult())
            {
                this->wait();
            }
            return ptr;
        }

        operator bool() const
        {
            return ptr != nullptr && impl != nullptr;
        }

      private:
        core::ref_ptr<Type> ptr;
        std::unique_ptr<FutureImpl> impl;
    };

    template <>
    class Future<void>
    {
        template <typename Derived>
        friend class Future;

      public:
        Future() : impl(nullptr)
        {
        }

        Future(std::unique_ptr<FutureImpl>&& impl) : impl(std::move(impl))
        {
        }

        Future(Future&& other) : impl(std::move(other.impl))
        {
        }

        template <typename Derived>
        Future(Future<Derived> other) : impl(std::move(other.impl))
        {
        }

        Future(Future const&) = delete;

        auto operator=(Future&& other) -> Future&
        {
            impl = std::move(other.impl);
            return *this;
        }

        template <typename Derived>
        auto operator=(Future<Derived> other) -> Future&
        {
            impl = std::move(other.impl);
            return *this;
        }

        auto operator=(Future const&) -> Future& = delete;

        auto getResult() const -> bool
        {
            if (impl)
            {
                return impl->getResult();
            }
            else
            {
                return true;
            }
        }

        auto wait() const -> void
        {
            if (impl)
            {
                impl->wait();
            }
        }

        operator bool() const
        {
            return impl != nullptr;
        }

        auto waitOnContext(IDeviceContext* context) const -> void
        {
            if (impl)
            {
                impl->waitOnContext(context);
            }
        }

      private:
        std::unique_ptr<FutureImpl> impl;
    };

    class Query : public core::ref_counted_object
    {
    };

    class IDeviceContext
    {
      public:
        virtual ~IDeviceContext() = default;

        virtual auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
            -> void = 0;

        virtual auto barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
            -> void = 0;

        virtual auto execute() -> Future<void> = 0;
    };

    class GraphicsContext : public IDeviceContext
    {
      public:
        virtual auto setGraphicsPipelineOptions(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                                BlendColorInfo const& blendColor,
                                                std::optional<DepthStencilStageInfo> const depthStencil) -> void = 0;

        virtual auto bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void = 0;

        virtual auto beginRenderPass(std::span<RenderPassColorInfo const> const colors,
                                     std::optional<RenderPassDepthStencilInfo> depthStencil) -> void = 0;

        virtual auto endRenderPass() -> void = 0;

        virtual auto bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
            -> void = 0;

        virtual auto bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                     IndexFormat const format) -> void = 0;

        virtual auto drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void = 0;

        virtual auto draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void = 0;

        virtual auto setViewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
            -> void = 0;

        virtual auto setScissor(int32_t const left, int32_t const top, int32_t const right, int32_t const bottom)
            -> void = 0;
    };

    class CopyContext : public IDeviceContext
    {
      public:
        virtual auto updateBuffer(core::ref_ptr<Buffer> dest, uint64_t const offset,
                                  std::span<uint8_t const> const dataBytes) -> Future<Buffer> = 0;

        virtual auto updateTexture(core::ref_ptr<Texture> dest, uint32_t const resourceIndex, uint8_t const* dataBytes) -> Future<Texture> = 0;

        /*
        virtual auto readBuffer(core::ref_ptr<Buffer> dest, uint8_t* dataBytes) -> size_t = 0;

        virtual auto readTexture(core::ref_ptr<Texture> dest, uint32_t const mipLevel, uint8_t* dataBytes)
            -> size_t = 0;
        */
    };

    class Swapchain
    {
      public:
        virtual ~Swapchain() = default;

        virtual auto getBackBuffer() -> core::weak_ptr<Texture> = 0;

        virtual auto presentBackBuffer() -> void = 0;

        virtual auto resizeBackBuffers(uint32_t const width, uint32_t const height) -> void = 0;
    };

    class RHI : public core::ref_counted_object
    {
      public:
        virtual ~RHI() = default;

        static auto create(RHICreateInfo const& rhiCreateInfo,
                           std::optional<SwapchainCreateInfo> const swapchainCreateInfo = std::nullopt)
            -> core::ref_ptr<RHI>;

        virtual auto createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader> = 0;

        virtual auto createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture> = 0;

        virtual auto createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer> = 0;

        virtual auto createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler> = 0;

        virtual auto getSwapchain() -> Swapchain* = 0;

        virtual auto getGraphicsContext() -> GraphicsContext* = 0;

        virtual auto getCopyContext() -> CopyContext* = 0;

        virtual auto getName() const -> std::string const& = 0;
    };
} // namespace ionengine::rhi