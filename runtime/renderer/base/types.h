// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Instance;
class Adapter;
class Device;
class CommandQueue;
class Swapchain;
class Shader;
class Pipeline;
class DescriptorSetLayout;
class RenderPass;

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
    RWStructuredBuffer
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
    std::vector<RenderPassColorDesc> colors;
    RenderPassDepthStencilDesc depth_stencil;
    uint32 sample_count;
};

struct InputLayoutDesc {
    uint32 slot;
    std::string semantic_name;
    Format format;
    uint32 stride;
};

struct ShaderStageDesc {
    std::shared_ptr<Shader> shader;
    ShaderType shader_type;
};

struct DescriptorSetLayoutBinding {
    ShaderType shader_type;
    ViewType view_type;
    uint32 slot;
    uint32 space;
    uint32 count;
};

struct RasterizerDesc {
    FillMode fill_mode;
    CullMode cull_mode;
    int32 depth_bias;
};

struct StencilOpDesc {
    StencilOp fail_op;
    StencilOp depth_fail_op;
    StencilOp pass_op;
    ComparisonFunc func;
};

struct DepthStencilDesc {
    bool depth_test_enable;
    ComparisonFunc depth_func;
    bool depth_write_enable;
    bool depth_bounds_test_enable;
    bool stencil_enable;
    uint8 stencil_read_mask;
    uint8 stencil_write_mask;
    StencilOpDesc front_face;
    StencilOpDesc back_face;
};

struct BlendDesc {
    bool blend_enable;
    Blend blend_src;
    Blend blend_dest;
    BlendOp blend_op;
    Blend blend_src_alpha;
    Blend blend_dest_alpha;
    BlendOp blend_op_alpha;
};

struct GraphicsPipelineDesc {

    std::vector<ShaderStageDesc> stages;
    std::shared_ptr<DescriptorSetLayout> layout;
    std::vector<InputLayoutDesc> inputs;
    std::shared_ptr<RenderPass> render_pass;
    RasterizerDesc rasterizer;
    DepthStencilDesc depth_stencil;
    BlendDesc blend;
};

}