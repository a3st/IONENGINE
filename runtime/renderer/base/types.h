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
    RenderPassLoadOp load_op = RenderPassLoadOp::Load;
    RenderPassStoreOp store_op  = RenderPassStoreOp::Store;
};

struct RenderPassDepthStencilDesc {
    Format format;
    RenderPassLoadOp depth_load_op = RenderPassLoadOp::Load;
    RenderPassStoreOp depth_store_op = RenderPassStoreOp::Store;
    RenderPassLoadOp stencil_load_op = RenderPassLoadOp::Load;
    RenderPassStoreOp stencil_store_op = RenderPassStoreOp::Store;
};

struct RenderPassDesc {
    std::vector<RenderPassColorDesc> colors;
    RenderPassDepthStencilDesc depth_stencil;
    uint32 sample_count = 1;
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

    std::vector<ShaderStageDesc> stages;
    std::shared_ptr<DescriptorSetLayout> layout;
    std::vector<InputLayoutDesc> inputs;
    std::shared_ptr<RenderPass> render_pass;
    RasterizerDesc rasterizer;
    DepthStencilDesc depth_stencil;
    BlendDesc blend;

    GraphicsPipelineDesc& set_stages(const std::vector<ShaderStageDesc>& descs) {
        stages = descs;
        return *this;
    }

    GraphicsPipelineDesc& set_layout(const std::shared_ptr<DescriptorSetLayout>& layout_) {
        layout = layout_;
        return *this;
    }

    GraphicsPipelineDesc& set_inputs(const std::vector<InputLayoutDesc>& descs) {
        inputs = descs;
        return *this;
    }

    GraphicsPipelineDesc& set_render_pass(const std::shared_ptr<RenderPass>& render_pass_) {
        render_pass = render_pass_;
        return *this;
    }

    GraphicsPipelineDesc& set_rasterizer(const RasterizerDesc& desc) {
        rasterizer = desc;
        return *this;
    }

    GraphicsPipelineDesc& set_depth_stencil(const DepthStencilDesc& desc) {
        depth_stencil = desc;
        return *this;
    }

    GraphicsPipelineDesc& set_blend(const BlendDesc& desc) {
        blend = desc;
        return *this;
    }
};

}