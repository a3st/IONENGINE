// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace lgfx {

class Device;
class Fence;
class TextureView;
class MemoryPool;
class Texture;

enum class MemoryType {
    kDefault,
    kUpload,
    kReadBack
};

enum class MemoryFlags {
    kBuffers,
    kRT_DS,
    kNon_RT_DS
};

enum class ComparisonFunc {
    kNever,
    kLess,
    kEqual,
    kLessEqual,
    kGreater,
    kNotEqual,
    kGreaterEqual,
    kAlways
};

enum class Filter {
    kAnisotropic,
    kMinMagMipLinear,
    kComparisonMinMagMipLinear
};

enum class TextureAddressMode {
    kWrap,
    kClamp
};

enum class BufferFlags {
    kUnorderedAccess = 1 << 0,
    kConstantBuffer = 1 << 1,
    kIndexBuffer = 1 << 2,
    kVertexBuffer = 1 << 3,
    kCopyDest = 1 << 4,
    kCopySource = 1 << 5
};

DECLARE_ENUM_CLASS_BIT_FLAG(BufferFlags)

enum class TextureFlags {
    kRenderTarget = 1 << 0,
    kDepthStencil = 1 << 1,
    kShaderResource = 1 << 2,
    kUnorderedAccess = 1 << 3
};

DECLARE_ENUM_CLASS_BIT_FLAG(TextureFlags)

enum class Dimension {
    kBuffer,
    kTexture1D,
    kTexture1DArray,
    kTexture2D,
    kTexture2DArray,
    kTexture3D,
    kTextureCube,
    kTextureCubeArray
};

enum class DescriptorType {
    kSampler,
    kRenderTarget,
    kDepthStencil,
    kShaderResource
};

enum class DescriptorFlags {
    kNone,
    kShaderVisible
};

enum class Format {
    kUnknown,
    kRGBA32float,
    kRGBA32uint,
    kRGBA32int,
    kRGB32float,
    kRGB32uint,
    kRGB32int,
    kRG32float,
    kRG32uint,
    kRG32int,
    kR32float,
    kR32uint,
    kR32int,
    kRGBA16float,
    kRGBA16uint,
    kRGBA16int,
    kRGBA16unorm,
    kRGBA16snorm,
    kRG16float,
    kRG16uint,
    kRG16int,
    kRG16unorm,
    kRG16snorm,
    kR16float,
    kR16uint,
    kR16int,
    kR16unorm,
    kR16snorm,
    kRGBA8uint,
    kRGBA8int,
    kRGBA8unorm,
    kRGBA8snorm,
    kRG8uint,
    kRG8int,
    kRG8unorm,
    kRG8snorm,
    kR8uint,
    kR8int,
    kR8unorm,
    kR8snorm
};

enum class CommandBufferType {
    kGraphics,
    kCopy,
    kCompute
};

enum class MemoryState {
    kCommon,
    kRenderTarget,
    kPresent
};

enum class RenderPassLoadOp {
    kLoad,
    kClear,
    kDontCare
};

enum class RenderPassStoreOp {
    kStore,
    kDontCare
};

struct TextureViewDesc {
    Dimension dimension;
    uint32_t base_mip_level;
    uint32_t mip_level_count;
    uint32_t base_array_layer;
    uint32_t array_layer_count;
};

struct RenderPassColorDesc {
    Format format;
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
};

struct RenderPassDepthStencilDesc {
    Format format;
    RenderPassLoadOp depth_load_op;
    RenderPassStoreOp depth_store_op;
    RenderPassLoadOp stencil_load_op;
    RenderPassStoreOp stencil_store_op;
};

struct RenderPassDesc {
    std::array<RenderPassColorDesc, 8> colors;
    RenderPassDepthStencilDesc depth_stencil;
    uint32_t sample_count;
};

struct FrameBufferDesc {
    RenderPassDesc* render_pass;
    uint32_t width;
    uint32_t height;
    std::array<TextureView*, 8> colors;
    TextureView* depth_stencil;
};

struct ClearValueColor {
    float r, g, b, a;
};

struct ClearValueDesc {
    std::array<ClearValueColor, 8> colors;
    float depth;
    uint8_t stencil;
};

}