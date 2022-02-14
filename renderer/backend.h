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
struct DescriptorLayout;
struct DescriptorSet;

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

enum class ResourceFlags : uint16_t {
    None = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
    UnorderedAccess = 1 << 3,
    ConstantBuffer = 1 << 4,
    VertexShader = 1 << 5,
    GeometryShader = 1 << 6,
    DomainShader = 1 << 7,
    PixelShader = 1 << 8,
    ComputeShader = 1 << 9,
    HullShader = 1 << 10,
    HostVisible = 1 << 11
};

DECLARE_ENUM_CLASS_BIT_FLAG(ResourceFlags)

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

enum class DescriptorBindType {
    ShaderResource,
    ConstantBuffer,
    UnorderedAccess,
    Sampler
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

struct DescriptorBindDesc {
    DescriptorBindType bind_type;
    uint32_t index;
    uint32_t count;
    ResourceFlags flags;
};

struct DescriptorUpdateDesc {
    uint32_t index;
    std::span<std::variant<Handle<Texture>, Handle<Buffer>, Handle<Sampler>>> data;
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
        ResourceFlags const flags
    );

    Handle<Buffer> create_buffer(size_t const size, ResourceFlags const flags);

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
        uint16_t const aniso,
        CompareOp const compare_op
    );

    Handle<Shader> create_shader(std::span<char8_t> const data, ResourceFlags const flags);

    Handle<DescriptorLayout> create_descriptor_layout(std::vector<DescriptorBindDesc> const& bindings);

    Handle<DescriptorSet> create_descriptor_set(Handle<DescriptorLayout> const& handle);

    Handle<Pipeline> create_pipeline(
        Handle<DescriptorLayout> const& layout_handle,
        std::vector<VertexInputDesc> const& vertex_inputs,
        std::vector<Handle<Shader>> const& shader_handles,
        RasterizerDesc const& rasterizer,
        DepthStencilDesc const& depth_stencil,
        BlendDesc const& blend,
        Handle<RenderPass> const& render_pass_handle
    );

    void update_descriptor_set(Handle<DescriptorSet> const& handle, std::vector<DescriptorUpdateDesc> const& updates);

    void copy_buffer_data(Handle<Buffer> const& handle, uint64_t const offset, std::span<char8_t> const data);

    void bind_vertex_buffer(uint32_t const index, Handle<Buffer> const& handle);

    void bind_index_buffer(Handle<Buffer> const& handle, Format const format);

    void barrier(std::variant<Handle<Texture>, Handle<Buffer>> const& handle, MemoryState const before, MemoryState const after);

    void bind_pipeline(Handle<Pipeline> const& handle);

    void bind_descriptor_set(Handle<DescriptorSet> const& handle);

    void set_viewport(uint32_t const x, uint32_t const y, uint32_t const width, uint32_t const height);

    void set_scissor(uint32_t const left, uint32_t const top, uint32_t const right, uint32_t const bottom);

    void begin_render_pass(Handle<RenderPass> const& handle, std::vector<Color> const& rtv_clears, std::pair<float, uint8_t> dsv_clear);

    void end_render_pass();

    void draw(uint32_t const vertex_index, uint32_t const vertex_count);

    Handle<Texture> begin_frame();

    void end_frame();

    void resize_frame(uint32_t const width, uint32_t const height, uint32_t const frame_count);

    void wait_for_idle_device();

private:

    struct Impl;
    std::unique_ptr<Impl> _impl;
};

}