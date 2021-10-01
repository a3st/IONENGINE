// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class Fence;
class CommandList;
class Resource;
class View;
class RenderPass;
class FrameBuffer;
class BindingSetLayout;
class BindingSet;
class Pipeline;
class Device;

enum class Format {
    Unknown,
    RGBA32float,
    RGBA32uint,
    RGBA32int,
    RGB32float,
    RGB32uint,
    RGB32int,
    RG32float,
    RG32uint,
    RG32int,
    R32float,
    R32uint,
    R32int,
    RGBA16float,
    RGBA16uint,
    RGBA16int,
    RGBA16unorm,
    RGBA16snorm,
    RG16float,
    RG16uint,
    RG16int,
    RG16unorm,
    RG16snorm,
    R16float,
    R16uint,
    R16int,
    R16unorm,
    R16snorm,
    RGBA8uint,
    RGBA8int,
    RGBA8unorm,
    RGBA8snorm,
    RG8uint,
    RG8int,
    RG8unorm,
    RG8snorm,
    R8uint,
    R8int,
    R8unorm,
    R8snorm
};

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
    One,
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

enum class ResourceState : uint32 {
    Common = 1 << 0,
    VertexAndConstantBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    RenderTarget = 1 << 3,
    UnorderedAccess = 1 << 4,
    DepthStencilWrite = 1 << 5,
    DepthStencilRead = 1 << 6,
    NonPixelShaderResource = 1 << 7,
    PixelShaderResource = 1 << 8,
    CopyDest = 1 << 9,
    CopySource = 1 << 10,
    Present = 1 << 11,
    GenericRead = 
        ResourceState::VertexAndConstantBuffer | 
        ResourceState::IndexBuffer |
        ResourceState::CopySource |
        ResourceState::NonPixelShaderResource |
        ResourceState::PixelShaderResource
};

ENUM_CLASS_BIT_FLAG_DECLARE(ResourceState)

enum class ResourceType {
    Unknown,
    Buffer,
    Texture
};

enum class ResourceFlags : uint32 {
    RenderTarget = 1 << 0,
    DepthStencil = 1 << 1,
    ShaderResource = 1 << 2,
    UnorderedAccess = 1 << 3,
    ConstantBuffer = 1 << 4,
    IndexBuffer = 1 << 5,
    VertexBuffer = 1 << 6,
    CopyDest = 1 << 7,
    CopySource = 1 << 8
};

ENUM_CLASS_BIT_FLAG_DECLARE(ResourceFlags)

enum class PipelineType {
    Graphics,
    Compute
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

struct AdapterDesc {
    std::string name;
    usize local_memory;
    uint32 vendor_id;
    uint32 device_id;
};

struct ShaderDesc {
    ShaderType shader_type;
    std::filesystem::path blob_path;
};

struct RenderPassColorDesc {
    Format format = Format::Unknown;
    RenderPassLoadOp load_op = RenderPassLoadOp::DontCare;
    RenderPassStoreOp store_op  = RenderPassStoreOp::DontCare;

    auto make_tie() const { return std::tie(format, load_op, store_op); }
};

STD_TIE_CMP_OPERATOR_DECLARE(RenderPassColorDesc)

struct RenderPassDepthStencilDesc {
    Format format = Format::Unknown;
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
    RenderPass* render_pass;
    uint32 width;
    uint32 height;
    std::vector<View*> colors;
    std::optional<View*> depth_stencil;
};

struct RenderPassDesc {
    std::vector<RenderPassColorDesc> colors;
    RenderPassDepthStencilDesc depth_stencil;
    uint32 sample_count = 1;
};

struct ResourceBarrierDesc {
    Resource* resource;
    ResourceState before;
    ResourceState after;
};

struct ClearValueColor {
    float r, g, b, a;
    auto make_tie() const { return std::tie(r, g, b, a); }
};

STD_TIE_CMP_OPERATOR_DECLARE(ClearValueColor)

struct ClearValueDesc {
    std::vector<ClearValueColor> colors;
    float depth = 0.0f;
    uint8 stencil = 0;
};

struct VertexInputDesc {
    std::string semantic_name;
    uint32 index;
    Format format = Format::Unknown;
    uint32 slot;
    uint32 stride;
};

struct BindingSetBinding {
    ShaderType shader_type;
    ViewType view_type = ViewType::Unknown;
    uint32 slot;
    uint32 space;
    uint32 count;
};

struct WriteBindingSet {
    uint32 slot;
    uint32 count;
    ViewType view_type = ViewType::Unknown;
    std::vector<View*> views;
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
    ViewDimension dimension = ViewDimension::Unknown;
    uint32 array_size;
    uint32 mip_slice;
    uint32 array_slice;
    uint32 plane_slice;
    uint32 structure_stride;
    uint64 buffer_size;
};

struct ResourceDesc {
    ViewDimension dimension = ViewDimension::Unknown;
    uint64 width;
    uint32 height;
    uint16 array_size;
    uint16 mip_levels;
    Format format = Format::Unknown;
    uint32 sample_count = 1;
    ResourceFlags flags;
};

struct SamplerDesc {
    Filter filter;
    TextureAddressMode mode;
    ComparisonFunc func;
};

struct BlendDesc {
    bool blend_enable = false;
    Blend blend_src = Blend::One;
    Blend blend_dest = Blend::Zero;
    BlendOp blend_op = BlendOp::Add;
    Blend blend_src_alpha = Blend::One;
    Blend blend_dest_alpha = Blend::Zero;
    BlendOp blend_op_alpha = BlendOp::Add;
};

struct GraphicsPipelineDesc {
    std::vector<ShaderDesc> shaders;
    BindingSetLayout* layout;
    std::vector<VertexInputDesc> vertex_inputs;
    RenderPass* render_pass;
    RasterizerDesc rasterizer;
    DepthStencilDesc depth_stencil;
    BlendDesc blend;
};

struct ComputePipelineDesc {
    ShaderDesc shader;
    BindingSetLayout* layout;
};

}