// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace lgfx {

class Device;
class Fence;
class TextureView;
class BufferView;
class MemoryPool;
class Texture;
class Buffer;
class RenderPass;
class FrameBuffer;
class Pipeline;
class DescriptorLayout;
class DescriptorSet;
class Shader;

enum class PipelineType {

    kGraphics,
    kCompute
};

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

enum class ShaderType {
    
    kVertex,
    kPixel,
    kGeometry,
    kHull,
    kDomain,
    kCompute,
    kAll
};

enum class BufferFlags : uint32_t {

    kUnorderedAccess = 1 << 0,
    kConstantBuffer = 1 << 1,
    kIndexBuffer = 1 << 2,
    kVertexBuffer = 1 << 3,
    kCopyDest = 1 << 4,
    kCopySource = 1 << 5
};

DECLARE_ENUM_CLASS_BIT_FLAG(BufferFlags)

enum class TextureFlags : uint32_t {

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
    kShaderResource,
    kConstantBuffer,
    kUnorderedAccess
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
    kPresent,
    kGenericRead
};

enum class StencilOp {

    kKeep,
    kZero,
    kReplace,
    kIncrSat,
    kDecrSat,
    kInvert,
    kIncr,
    kDecr
};

enum class Blend {

    kZero,
    kOne,
    kSrcAlpha,
    kInvSrcAlpha
};

enum class BlendOp {

    kAdd,
    kSubtract,
    kRevSubtract,
    kMin,
    kMax
};

enum class FillMode {

    kWireframe,
    kSolid
};

enum class CullMode {

    kNone,
    kFront,
    kBack
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

struct DescriptorLayoutBinding {

    DescriptorType type;
    uint32_t slot;
    uint32_t space;
    uint32_t count;
    ShaderType shader_visible;

    inline bool operator<(const DescriptorLayoutBinding& rhs) const {

        return std::tie(type, slot, space, count, shader_visible) < std::tie(rhs.type, rhs.slot, rhs.space, rhs.count, rhs.shader_visible);
    }
};

struct InputLayoutDesc {

    std::string semantic;
    uint32_t index;
    Format format = Format::kUnknown;
    uint32_t slot;
    uint32_t stride;

    inline bool operator<(const InputLayoutDesc& rhs) const {

        return std::tie(semantic, index, format, slot, stride) < std::tie(rhs.semantic, rhs.index, rhs.format, rhs.slot, rhs.stride);
    }
};

struct BlendDesc {

    bool blend_enable = false;
    Blend blend_src = Blend::kOne;
    Blend blend_dst = Blend::kZero;
    BlendOp blend_op = BlendOp::kAdd;
    Blend blend_src_alpha = Blend::kOne;
    Blend blend_dst_alpha = Blend::kZero;
    BlendOp blend_op_alpha = BlendOp::kAdd;

    inline bool operator<(const BlendDesc& rhs) const {

        return std::tie(blend_enable, blend_src, blend_dst, blend_op, blend_src_alpha, blend_dst_alpha, blend_op_alpha) < 
            std::tie(rhs.blend_enable, rhs.blend_src, rhs.blend_dst, rhs.blend_op, rhs.blend_src_alpha, rhs.blend_dst_alpha, rhs.blend_op_alpha);
    }
};

struct StencilOpDesc {

    StencilOp fail_op = StencilOp::kKeep;
    StencilOp depth_fail_op = StencilOp::kKeep;
    StencilOp pass_op = StencilOp::kKeep;
    ComparisonFunc func = ComparisonFunc::kAlways;

    inline bool operator<(const StencilOpDesc& rhs) const {

        return std::tie(fail_op, depth_fail_op, pass_op, func) < 
            std::tie(rhs.fail_op, rhs.depth_fail_op, rhs.pass_op, rhs.func);
    }
};

struct DepthStencilDesc {

    bool depth_test_enable = false;
    ComparisonFunc depth_func = ComparisonFunc::kLess;
    bool depth_write_enable = true;
    bool depth_bounds_test_enable = false;
    bool stencil_enable = false;
    uint8_t stencil_read_mask = 0xff;
    uint8_t stencil_write_mask = 0xff;
    StencilOpDesc front_face;
    StencilOpDesc back_face;

    inline bool operator<(const DepthStencilDesc& rhs) const {

        return std::tie(depth_test_enable, depth_func, depth_write_enable, depth_bounds_test_enable, stencil_enable, stencil_read_mask, stencil_write_mask, front_face, back_face) < 
            std::tie(rhs.depth_test_enable, rhs.depth_func, rhs.depth_write_enable, rhs.depth_bounds_test_enable, rhs.stencil_enable, rhs.stencil_read_mask, rhs.stencil_write_mask, rhs.front_face, rhs.back_face);
    }
};

struct RasterizerDesc {

    FillMode fill_mode = FillMode::kSolid;
    CullMode cull_mode = CullMode::kBack;
    int32_t depth_bias = 0;

    inline bool operator<(const RasterizerDesc& rhs) const {

        return std::tie(fill_mode, cull_mode, depth_bias) < std::tie(rhs.fill_mode, rhs.cull_mode, rhs.depth_bias);
    }
};

struct RenderPassColorDesc {

    Format format = Format::kUnknown;
    RenderPassLoadOp load_op = RenderPassLoadOp::kDontCare;
    RenderPassStoreOp store_op = RenderPassStoreOp::kDontCare;

    inline bool operator<(const RenderPassColorDesc& rhs) const {

        return std::tie(format, load_op, store_op) < std::tie(rhs.format, rhs.load_op, rhs.store_op);
    }
};

struct RenderPassDepthStencilDesc {

    Format format = Format::kUnknown;
    RenderPassLoadOp depth_load_op = RenderPassLoadOp::kDontCare;
    RenderPassStoreOp depth_store_op = RenderPassStoreOp::kDontCare;
    RenderPassLoadOp stencil_load_op = RenderPassLoadOp::kDontCare;
    RenderPassStoreOp stencil_store_op = RenderPassStoreOp::kDontCare;

    inline bool operator<(const RenderPassDepthStencilDesc& rhs) const {

        return std::tie(format, depth_load_op, depth_store_op, stencil_load_op, stencil_store_op) < 
            std::tie(rhs.format, rhs.depth_load_op, rhs.depth_store_op, rhs.stencil_load_op, rhs.stencil_store_op);
    }
};

struct ClearValueColor {

    float r, g, b, a;

    inline bool operator<(const ClearValueColor& rhs) const {

        return std::tie(r, g, b, a) < std::tie(rhs.r, rhs.g, rhs.b, rhs.a);
    }
};

}