// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/window.h>
#include <renderer/color.h>
#include <lib/handle.h>

namespace ionengine::renderer {

struct Buffer;
struct Texture;
struct Pipeline;
struct Shader;
struct RenderPass;
struct Sampler;

enum class Dimension {
    _1D,
    _2D,
    _3D,
    Cube
};

enum class Format {
    Unknown,
    RGBA8Unorm,
    RGBA32,
    RGB32
};

enum class ResourceFlags {
    None,
    RenderTarget,
    DepthStencil,
    UnorderedAccess
};

enum class QueueType {
    Graphics,
    Copy,
    Compute
};

enum class PipelineType {
    Graphics,
    Compute
};

enum class RenderPassLoadOp {
    Load,
    Clear,
    DontCare
};

enum class MemoryState {
    Common,
    Present,
    RenderTarget
};

enum class RenderPassStoreOp {
    Store,
    DontCare
};

enum class ShaderBindType {
    ShaderResource,
    ConstantBuffer,
    UnorderedAccess,
    Sampler
};

enum class ShaderFlags : uint16_t {
    Vertex = 1 << 0,
    Geometry = 1 << 1,
    Domain = 1 << 2,
    Pixel = 1 << 3,
    Compute = 1 << 4,
    Hull = 1 << 5,
    All = Vertex | Geometry | Domain | Pixel | Compute | Hull
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

enum class CompareOp {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
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

enum class Filter {

};

enum class AddressMode {
    
};

struct Extent2D {
    uint32_t width;
    uint32_t height;
};

struct TextureViewDesc {
    uint16_t base_mip_level;
    uint16_t mip_level_count;
    uint16_t base_array_layer;
    uint16_t array_layer_count;
};

struct BufferViewDesc {
    uint16_t dummy;
};

struct RenderPassColorDesc {
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
};

struct RenderPassDepthStencilDesc {
    RenderPassLoadOp depth_load_op;
    RenderPassStoreOp depth_store_op;
    RenderPassLoadOp stencil_load_op;
    RenderPassStoreOp stencil_store_op;
};

struct ShaderBindDesc {
    ShaderBindType bind_type;
    uint32_t index;
    uint32_t count;
    ShaderFlags shader_flags;

    bool operator<(ShaderBindDesc const& other) const {
        return std::tie(bind_type, index, count, shader_flags) < std::tie(other.bind_type, other.index, other.count, other.shader_flags);
    }
};

struct VertexInputDesc {
    std::string semantic;
    uint32_t index;
    Format format;
    uint32_t slot;
    uint32_t stride;
};

struct RasterizerDesc {
    FillMode fill_mode;
    CullMode cull_mode;
};

struct DepthStencilDesc {
    CompareOp depth_func;
    bool write_enable;
};

struct BlendDesc {
    bool blend_enable;
    Blend blend_src;
    Blend blend_dst;
    BlendOp blend_op;
    Blend blend_src_alpha;
    Blend blend_dst_alpha;
    BlendOp blend_op_alpha;
};

using GPUResourceHandle = std::variant<Handle<Texture>, Handle<Buffer>>;

class Backend {
public:

    Backend(uint32_t const adapter_index, platform::Window* const window, uint32_t const frame_count);

    ~Backend();

    Backend(Backend const&) = delete;

    Backend(Backend&&) = delete;

    Backend& operator=(Backend const&) = delete;

    Backend& operator=(Backend&&) = delete;

    Handle<Texture> create_texture(
        Dimension const dimension,
        Extent2D const extent,
        uint16_t const mip_levels,
        uint16_t const array_layers,
        Format const format,
        ResourceFlags const flags,
        TextureViewDesc const& texture_view
    );

    Handle<Buffer> create_buffer(
        size_t const size,
        ResourceFlags const flags,
        BufferViewDesc const& buffer_view
    );

    Handle<RenderPass> create_render_pass(
        std::vector<Handle<Texture>> const& rtv_handles,
        std::vector<RenderPassColorDesc> const& rtv_ops,
        Handle<Texture> const& dsv_handle,
        RenderPassDepthStencilDesc const& dsv_op
    );

    Handle<Sampler> create_sampler(
        Filter const filter,
        AddressMode const address_u,
        AddressMode const address_v,
        AddressMode const address_w,
        uint32_t const anisotropy,
        CompareOp const compare_op
    );

    Handle<Shader> create_shader(
        std::vector<ShaderBindDesc> const& bindings,
        std::span<char8_t> const data, 
        ShaderFlags const flags
    );

    Handle<Pipeline> create_pipeline(
        std::vector<VertexInputDesc> const& vertex_inputs,
        std::vector<Handle<Shader>> const& shader_handles,
        RasterizerDesc const& rasterizer,
        DepthStencilDesc const& depth_stencil,
        BlendDesc const& blend,
        Handle<RenderPass> const& render_pass_handle
    );

    void barrier(GPUResourceHandle const& handle, MemoryState const before, MemoryState const after);

    void bind_pipeline(Handle<Pipeline> const& handle);

    void set_viewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);

    void set_scissor(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);

    void begin_render_pass(Handle<RenderPass> const& handle);

    void end_render_pass();

    Handle<Texture> begin_frame();

    void end_frame();

    void resize_frame(uint32_t const width, uint32_t const height, uint32_t const frame_count);

    void wait_for_idle_device();

private:

    struct Impl;
    std::unique_ptr<Impl> _impl;
};

}