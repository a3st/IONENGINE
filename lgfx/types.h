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

enum class ShaderModuleType {
    
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

struct TextureViewDesc {

    Dimension dimension;
    uint32_t base_mip_level;
    uint32_t mip_level_count;
    uint32_t base_array_layer;
    uint32_t array_layer_count;

    inline bool operator==(const TextureViewDesc& rhs) const {
        
        return dimension == rhs.dimension && base_mip_level == rhs.base_mip_level && mip_level_count == rhs.mip_level_count && base_array_layer == rhs.base_array_layer && array_layer_count == rhs.array_layer_count;
    }

    inline bool operator!=(const TextureViewDesc& rhs) const {
        
        return dimension != rhs.dimension || base_mip_level != rhs.base_mip_level || mip_level_count != rhs.mip_level_count || base_array_layer != rhs.base_array_layer || array_layer_count != rhs.array_layer_count;
    }

    inline bool operator<(const TextureViewDesc& rhs) const {
        
        return dimension < rhs.dimension || base_mip_level < rhs.base_mip_level || mip_level_count < rhs.mip_level_count || base_array_layer < rhs.base_array_layer || array_layer_count < rhs.array_layer_count;
    }

    inline bool operator>(const TextureViewDesc& rhs) const {
        
        return dimension > rhs.dimension || base_mip_level > rhs.base_mip_level || mip_level_count > rhs.mip_level_count || base_array_layer > rhs.base_array_layer || array_layer_count > rhs.array_layer_count;
    }
};

struct BufferDesc {

    size_t size;
    BufferFlags flags;
};

struct BufferViewDesc {

    Format index_format;
    uint32_t stride;
};

struct DescriptorLayoutBinding {

    DescriptorType type;
    uint32_t slot;
    uint32_t space;
    uint32_t count;
    ShaderModuleType shader_visible;
};

struct InputLayoutDesc {

    std::string_view semantic;
    uint32_t index;
    Format format = Format::kUnknown;
    uint32_t slot;
    uint32_t stride;
};

struct BlendDesc {

    bool blend_enable = false;
    Blend blend_src = Blend::kOne;
    Blend blend_dest = Blend::kZero;
    BlendOp blend_op = BlendOp::kAdd;
    Blend blend_src_alpha = Blend::kOne;
    Blend blend_dest_alpha = Blend::kZero;
    BlendOp blend_op_alpha = BlendOp::kAdd;
};

struct StencilOpDesc {

    StencilOp fail_op = StencilOp::kKeep;
    StencilOp depth_fail_op = StencilOp::kKeep;
    StencilOp pass_op = StencilOp::kKeep;
    ComparisonFunc func = ComparisonFunc::kAlways;
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
};

struct RasterizerDesc {

    FillMode fill_mode = FillMode::kSolid;
    CullMode cull_mode = CullMode::kBack;
    int32_t depth_bias = 0;
};

struct ShaderModuleDesc {

    ShaderModuleType type;
    std::span<std::byte> shader_code;
};

struct PipelineDesc {

    PipelineType type;
    DescriptorLayout* layout;
    std::span<InputLayoutDesc> inputs;
    std::span<ShaderModuleDesc> shaders;
    RasterizerDesc rasterizer;
    DepthStencilDesc depth_stencil;
    BlendDesc blend;
    RenderPass* render_pass;
};

struct TextureDesc {
    
    Dimension dimension;
    uint32_t width;
    uint32_t height;
    uint32_t mip_levels;
    uint32_t array_layers;
    Format format;
    TextureFlags flags;

    inline bool operator==(const TextureDesc& rhs) const {
        
        return dimension == rhs.dimension && width == rhs.width && height == rhs.height && mip_levels == rhs.mip_levels && array_layers == rhs.array_layers && format == rhs.format && flags == rhs.flags;
    }

    inline bool operator!=(const TextureDesc& rhs) const {
        
        return dimension != rhs.dimension || width != rhs.width || height != rhs.height || mip_levels != rhs.mip_levels || array_layers != rhs.array_layers || format != rhs.format || flags != rhs.flags;
    }

    inline bool operator<(const TextureDesc& rhs) const {
        
        return dimension < rhs.dimension || width < rhs.width || height < rhs.height || mip_levels < rhs.mip_levels || array_layers < rhs.array_layers || format < rhs.format || flags < rhs.flags;
    }

    inline bool operator>(const TextureDesc& rhs) const {
        
        return dimension > rhs.dimension || width > rhs.width || height > rhs.height || mip_levels > rhs.mip_levels || array_layers > rhs.array_layers || format > rhs.format || flags > rhs.flags;
    }
};

struct RenderPassColorDesc {

    Format format = Format::kUnknown;
    RenderPassLoadOp load_op = RenderPassLoadOp::kDontCare;
    RenderPassStoreOp store_op = RenderPassStoreOp::kDontCare;

    inline bool operator==(const RenderPassColorDesc& rhs) const {
        
        return format == rhs.format && load_op == rhs.load_op && store_op == rhs.store_op;
    }

    inline bool operator!=(const RenderPassColorDesc& rhs) const {
        
        return format != rhs.format || load_op != rhs.load_op || store_op != rhs.store_op;
    }

    inline bool operator<(const RenderPassColorDesc& rhs) const {
        
        if(load_op != rhs.load_op) {
            return load_op < rhs.load_op;
        }
        if(store_op != rhs.store_op) {
            return store_op < rhs.store_op;
        }
        return format < rhs.format;
    }

    inline bool operator>(const RenderPassColorDesc& rhs) const {
        
        if(load_op != rhs.load_op) {
            return load_op > rhs.load_op;
        }
        if(store_op != rhs.store_op) {
            return store_op > rhs.store_op;
        }
        return format > rhs.format;
    }
};

struct RenderPassDepthStencilDesc {

    Format format = Format::kUnknown;
    RenderPassLoadOp depth_load_op = RenderPassLoadOp::kDontCare;
    RenderPassStoreOp depth_store_op = RenderPassStoreOp::kDontCare;
    RenderPassLoadOp stencil_load_op = RenderPassLoadOp::kDontCare;
    RenderPassStoreOp stencil_store_op = RenderPassStoreOp::kDontCare;

    inline bool operator==(const RenderPassDepthStencilDesc& rhs) const {
        
        return format == rhs.format && depth_load_op == rhs.depth_load_op && depth_store_op == rhs.depth_store_op && stencil_load_op == rhs.stencil_load_op && stencil_store_op == rhs.stencil_store_op;
    }

    inline bool operator!=(const RenderPassDepthStencilDesc& rhs) const {
        
        return format != rhs.format && depth_load_op != rhs.depth_load_op || depth_store_op != rhs.depth_store_op || stencil_load_op != rhs.stencil_load_op || stencil_store_op != rhs.stencil_store_op;
    }

    inline bool operator<(const RenderPassDepthStencilDesc& rhs) const {
        
        if(depth_load_op != rhs.depth_load_op) {
            return depth_load_op < rhs.depth_load_op;
        }
        if(depth_store_op != rhs.depth_store_op) {
            return depth_store_op < rhs.depth_store_op;
        }
        if(stencil_load_op != rhs.stencil_load_op) {
            return stencil_load_op < rhs.stencil_load_op;
        }
        if(stencil_store_op != rhs.stencil_store_op) {
            return stencil_store_op < rhs.stencil_store_op;
        }
        return format < rhs.format;
    }

    inline bool operator>(const RenderPassDepthStencilDesc& rhs) const {
        
        if(depth_load_op != rhs.depth_load_op) {
            return depth_load_op > rhs.depth_load_op;
        }
        if(depth_store_op != rhs.depth_store_op) {
            return depth_store_op > rhs.depth_store_op;
        }
        if(stencil_load_op != rhs.stencil_load_op) {
            return stencil_load_op > rhs.stencil_load_op;
        }
        if(stencil_store_op != rhs.stencil_store_op) {
            return stencil_store_op > rhs.stencil_store_op;
        }
        return format > rhs.format;
    }
};

struct RenderPassDesc {

    std::span<RenderPassColorDesc> colors;
    RenderPassDepthStencilDesc depth_stencil;
    uint32_t sample_count = 1;

    inline bool operator==(const RenderPassDesc& rhs) const {
        
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin()) && depth_stencil == rhs.depth_stencil && sample_count == rhs.sample_count;
    }

    inline bool operator!=(const RenderPassDesc& rhs) const {
        
        return !std::equal(colors.begin(), colors.end(), rhs.colors.begin()) || depth_stencil != rhs.depth_stencil || sample_count != rhs.sample_count;
    }

    inline bool operator<(const RenderPassDesc& rhs) const {
        
        if(depth_stencil != rhs.depth_stencil) {
            return depth_stencil < rhs.depth_stencil;
        }
        if(sample_count != rhs.sample_count) {
            return sample_count < rhs.sample_count;
        }
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin(), [](auto& lhs, auto& rhs) -> bool { return lhs < rhs; });
    }

    inline bool operator>(const RenderPassDesc& rhs) const {
        
        if(depth_stencil != rhs.depth_stencil) {
            return depth_stencil > rhs.depth_stencil;
        }
        if(sample_count != rhs.sample_count) {
            return sample_count > rhs.sample_count;
        }
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin(), [](auto& lhs, auto& rhs) -> bool { return lhs > rhs; });
    }
};

struct FrameBufferDesc {

    RenderPass* render_pass;
    uint32_t width;
    uint32_t height;
    std::span<TextureView*> colors;
    TextureView* depth_stencil;

    inline bool operator==(const FrameBufferDesc& rhs) const {
        
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin()) && render_pass == rhs.render_pass && width == rhs.width && height == rhs.height && depth_stencil == rhs.depth_stencil;
    }

    inline bool operator!=(const FrameBufferDesc& rhs) const {
        
        return !std::equal(colors.begin(), colors.end(), rhs.colors.begin()) || render_pass != rhs.render_pass || width != rhs.width || height != rhs.height || depth_stencil != rhs.depth_stencil;
    }

    inline bool operator<(const FrameBufferDesc& rhs) const {
        
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin(), [](auto& lhs, auto& rhs) -> bool { return lhs < rhs; }) || render_pass < rhs.render_pass || width < rhs.width || height < rhs.height || depth_stencil < rhs.depth_stencil;
    }

    inline bool operator>(const FrameBufferDesc& rhs) const {
        
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin(), [](auto& lhs, auto& rhs) -> bool { return lhs > rhs; }) || render_pass > rhs.render_pass || width > rhs.width || height > rhs.height || depth_stencil > rhs.depth_stencil;
    }
};

struct ClearValueColor {

    float r, g, b, a;

    inline bool operator==(const ClearValueColor& rhs) const {
        
        return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
    }

    inline bool operator!=(const ClearValueColor& rhs) const {
        
        return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
    }

    inline bool operator<(const ClearValueColor& rhs) const {
        
        return r < rhs.r || g < rhs.g || b < rhs.b || a < rhs.a;
    }

    inline bool operator>(const ClearValueColor& rhs) const {
        
        return r > rhs.r || g > rhs.g || b > rhs.b || a > rhs.a;
    }
};

struct ClearValueDesc {

    std::span<ClearValueColor> colors;
    float depth;
    uint8_t stencil;

    inline bool operator==(const ClearValueDesc& rhs) const {
        
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin()) && depth == rhs.depth && stencil == rhs.stencil;
    }

    inline bool operator!=(const ClearValueDesc& rhs) const {
        
        return !std::equal(colors.begin(), colors.end(), rhs.colors.begin()) || depth != rhs.depth || stencil != rhs.stencil;
    }

    inline bool operator<(const ClearValueDesc& rhs) const {
        
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin(), [](auto& lhs, auto& rhs) -> bool { return lhs < rhs; }) || depth < rhs.depth || stencil < rhs.stencil;
    }

    inline bool operator>(const ClearValueDesc& rhs) const {
        
        return std::equal(colors.begin(), colors.end(), rhs.colors.begin(), [](auto& lhs, auto& rhs) -> bool { return lhs > rhs; }) || depth > rhs.depth || stencil > rhs.stencil;
    }
};

}