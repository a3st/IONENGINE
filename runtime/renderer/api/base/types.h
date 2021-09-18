// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class Instance;
class Adapter;
class Device;
class CommandQueue;
class Swapchain;
class Shader;
class Pipeline;
class DescriptorSetLayout;
class DescriptorSet;
class DescriptorPool;
class RenderPass;
class Resource;
class Memory;
class Fence;
class View;

enum class Format;

enum class ShaderType {
    Vertex,
    Pixel,
    Geometry,
    Compute,
    Hull,
    Domain,
    All
};

enum class ViewDimension {
    Unknown,
    Buffer,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture3D,
    TextureCube,
    TextureCubeArray
};

enum class ViewType {
    Unknown,
    ConstantBuffer,
    Sampler,
    Texture,
    RWTexture,
    Buffer,
    RWBuffer,
    StructuredBuffer,
    RWStructuredBuffer,
    RenderTarget,
    DepthStencil
};

enum class CommandListType {
    Graphics,
    Copy,
    Compute
};

enum class TextureType {
    _1D,
    _2D,
    _3D
};

enum class FillMode {
    Wireframe,
    Solid
};

enum class CullMode {
    None,
    Front,
    Back
};

enum ComparisonFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

enum class MemoryType {
    Default,
    Upload,
    Readback
};

enum class StencilOp {
    Keep,
    Zero,
    Replace,
    IncrSat,
    DecrSat,
    Invert,
    Incr,
    Decr
};

enum class Blend {
    Zero,
    SrcAlpha,
    InvSrcAlpha
};

enum class BlendOp {
    Add,
    Subtract,
    RevSubtract,
    Min,
    Max
};

enum class RenderPassLoadOp {
    Load,
    Clear,
    DontCare
};

enum class RenderPassStoreOp {
    Store,
    DontCare
};

enum class ResourceState {
    Common = 1 << 0,
    VertexAndConstantBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    RenderTarget = 1 << 3,
    UnorderedAccess = 1 << 4,
    DepthStencilWrite = 1 << 5,
    DepthStencilRead = 1 << 6,
    NonPixelShaderResource = 1 << 7,
    PixelShaderResource = 1 << 8,
    IndirectArgument = 1 << 9,
    CopyDest = 1 << 10,
    CopySource = 1 << 11,
    Present = 1 << 14,
    GenericRead =
            ResourceState::VertexAndConstantBuffer |
            ResourceState::IndexBuffer |
            ResourceState::CopySource |
            ResourceState::NonPixelShaderResource |
            ResourceState::PixelShaderResource |
            ResourceState::IndirectArgument
};

ENUM_CLASS_BIT_FLAG_DECLARE(ResourceState)

enum class ResourceType {
    Unknown,
    Buffer,
    Texture,
    Sampler
};

enum class ResourceFlags : uint32 {
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
    ShaderResource = 1 << 3,
    UnorderedAccess = 1 << 4,
    ConstantBuffer = 1 << 5,
    IndexBuffer = 1 << 6,
    VertexBuffer = 1 << 7,
    CopyDest = 1 << 8,
    CopySource = 1 << 9
};

enum class PipelineType {
    Graphics,
    Compute
};

ENUM_CLASS_BIT_FLAG_DECLARE(ResourceFlags)

struct RenderPassColorDesc {
    Format format;
    RenderPassLoadOp load_op = RenderPassLoadOp::DontCare;
    RenderPassStoreOp store_op  = RenderPassStoreOp::DontCare;

    auto make_tie() const { 
        return std::tie(format, load_op, store_op); 
    }
};

STD_TIE_CMP_OPERATOR_DECLARE(RenderPassColorDesc)

struct RenderPassDepthStencilDesc {
    Format format;
    RenderPassLoadOp depth_load_op = RenderPassLoadOp::DontCare;
    RenderPassStoreOp depth_store_op = RenderPassStoreOp::DontCare;
    RenderPassLoadOp stencil_load_op = RenderPassLoadOp::DontCare;
    RenderPassStoreOp stencil_store_op = RenderPassStoreOp::DontCare;

    auto make_tie() const { 
        return std::tie(format, depth_load_op, depth_store_op, stencil_load_op, stencil_store_op); 
    }
};

STD_TIE_CMP_OPERATOR_DECLARE(RenderPassDepthStencilDesc)

struct FrameBufferDesc {
    std::reference_wrapper<RenderPass> render_pass;
    uint32 width;
    uint32 height;
    std::vector<std::reference_wrapper<View>> colors;
    std::optional<std::reference_wrapper<View>> depth_stencil;
};

struct RenderPassDesc {
    std::vector<RenderPassColorDesc> colors;
    RenderPassDepthStencilDesc depth_stencil;
    uint32 sample_count = 1;
};

struct ResourceBarrierDesc {
    std::reference_wrapper<Resource> resource;
    ResourceState state_before;
    ResourceState state_after;
};

struct ClearValueColor {
    float r;
    float g;
    float b;
    float a;

    auto make_tie() const {
        return std::tie(r, g, b, a);
    }
};

STD_TIE_CMP_OPERATOR_DECLARE(ClearValueColor)

struct ClearValueDesc {
    std::vector<ClearValueColor> colors;
    float depth = 0.0f;
    uint8 stencil = 0;
};

struct InputLayoutDesc {
    std::string semantic_name;
    uint32 index;
    Format format;
    uint32 slot;
    uint32 stride;
};

struct DescriptorSetLayoutBinding {
    ShaderType shader_type;
    ViewType view_type;
    uint32 slot;
    uint32 space;
    uint32 count;
};

struct RasterizerDesc {
    FillMode fill_mode = FillMode::Solid;
    CullMode cull_mode = CullMode::Back;
    int32 depth_bias = 0;
};

struct StencilOpDesc {
    StencilOp fail_op = StencilOp::Keep;
    StencilOp depth_fail_op = StencilOp::Keep;
    StencilOp pass_op = StencilOp::Keep;
    ComparisonFunc func = ComparisonFunc::Always;
};

struct DepthStencilDesc {
    bool depth_test_enable = false;
    ComparisonFunc depth_func = ComparisonFunc::Less;
    bool depth_write_enable = true;
    bool depth_bounds_test_enable = false;
    bool stencil_enable = false;
    uint8 stencil_read_mask = 0xff;
    uint8 stencil_write_mask = 0xff;
    StencilOpDesc front_face;
    StencilOpDesc back_face;
};

struct ViewDesc {
    ViewType view_type;
    ViewDimension dimension;
    uint32 array_size;
    uint32 mip_slice;
    uint32 array_slice;
    uint32 plane_slice;
    uint32 structure_stride;
    uint64 buffer_size;
};

struct ResourceDesc {
    ViewDimension dimension;
    uint64 width;
    uint32 height;
    uint16 array_size;
    uint16 mip_levels;
    Format format;
    uint32 sample_count;
    ResourceFlags flags;
};

enum class Filter {
    Anisotropic,
    MinMagMipLinear,
    ComparisonMinMagMipLinear
};

enum class TextureAddressMode {
    Wrap,
    Clamp
};

struct SamplerDesc {
    Filter filter;
    TextureAddressMode mode;
    ComparisonFunc func;
};

struct BlendDesc {
    bool blend_enable = false;
    Blend blend_src;
    Blend blend_dest;
    BlendOp blend_op;
    Blend blend_src_alpha;
    Blend blend_dest_alpha;
    BlendOp blend_op_alpha;
};

struct GraphicsPipelineDesc {
    std::vector<std::reference_wrapper<Shader>> shaders;
    std::reference_wrapper<DescriptorSetLayout> layout;
    std::vector<InputLayoutDesc> inputs;
    std::reference_wrapper<RenderPass> render_pass;
    RasterizerDesc rasterizer;
    DepthStencilDesc depth_stencil;
    BlendDesc blend;
};

struct DescriptorPoolSize {
    ViewType type;
    uint32 count;
};

}