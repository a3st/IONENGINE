// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/handle.h>
#include <platform/window.h>
#include <renderer/color.h>

namespace ionengine::renderer::backend {

struct Buffer;
struct Texture;
struct Pipeline;
struct Shader;
struct RenderPass;
struct Sampler;
struct DescriptorLayout;
struct DescriptorSet;
struct CachePipeline;
struct CommandList;

enum class BackendLimits : uint32_t {
    RenderPassCount = 256,
    PipelineCount = 256,
    ShaderCount = 512,
    DescriptorLayoutCount = 4,
    TextureCount = 4096,
    SamplerCount = 128,
    DescriptorSetCount = 512,
    BufferCount = 512,
    EncoderCount = 24,
    DeviceCount = 3,
    BufferedSwapCount = 3
};

enum class Dimension {
    _1D,
    _2D,
    _3D,
    Cube
};

enum class Format {
    Unknown,
    BGRA8,
    BGR8,
    RGBA8,
    RGBA32,
    RGB32,
    RG32,
    R32,
    BC1,
    BC5,
    D32
};

enum class TextureFlags : uint8_t {
    ShaderResource = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
    UnorderedAccess = 1 << 3,
};

DECLARE_ENUM_CLASS_BIT_FLAG(TextureFlags)

enum class BufferFlags : uint16_t {
    ConstantBuffer = 1 << 0,
    VertexBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    UnorderedAccess = 1 << 3,
    HostWrite = 1 << 4,
    HostRead = 1 << 5
};

DECLARE_ENUM_CLASS_BIT_FLAG(BufferFlags)

enum class ShaderFlags : uint16_t {
    Vertex = 1 << 0,
    Geometry = 1 << 1,
    Domain = 1 << 2,
    Hull = 1 << 3,
    Pixel = 1 << 4,
    Compute = 1 << 5,
    All = Vertex | Geometry | Domain | Hull | Pixel | Compute
};

DECLARE_ENUM_CLASS_BIT_FLAG(ShaderFlags)

enum class QueueFlags : uint8_t {
    Graphics = 1 << 0,
    Copy = 1 << 1,
    Compute = 1 << 2
};

DECLARE_ENUM_CLASS_BIT_FLAG(QueueFlags)

enum class RenderPassLoadOp {
    Load,
    Clear,
    DontCare
};

enum class RenderPassStoreOp {
    Store,
    DontCare
};

enum class MemoryState : uint32_t {
    Common = 1 << 0,
    Present = 1 << 1,
    RenderTarget = 1 << 2,
    CopySource = 1 << 3,
    CopyDest = 1 << 4,
    ShaderRead = 1 << 5,
    VertexConstantBufferRead = 1 << 6,
    IndexBufferRead = 1 << 7,
    DepthRead = 1 << 8,
    DepthWrite = 1 << 9
};

enum class DescriptorRangeType {
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
    Anisotropic,
    MinMagMipLinear,
    ComparisonMinMagMipLinear
};

enum class AddressMode {
    Wrap,
    Clamp,
    Mirror
};

using ResourceHandle = std::variant<Handle<Texture>, Handle<Buffer>, Handle<Sampler>>;

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

struct DescriptorRangeDesc {
    DescriptorRangeType range_type;
    uint32_t index;
    uint32_t count;
    ShaderFlags flags;
};

struct DescriptorWriteDesc {
    uint32_t index;
    ResourceHandle data;
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

struct AdapterDesc {
    std::string name;
};

struct SwapchainDesc {
    platform::Window* window;
    uint16_t sample_count;
    uint32_t buffer_count;
};

class Device {

    friend class Encoder;

public:

    Device(uint32_t const adapter_index, SwapchainDesc const& swapchain_desc);

    ~Device();

    Device(Device const&) = delete;

    Device(Device&&) noexcept;

    Device& operator=(Device const&) = delete;

    Device& operator=(Device&&) noexcept;

    Handle<Texture> create_texture(
        Dimension const dimension,
        uint32_t const width,
        uint32_t const height,
        uint16_t const mip_levels,
        uint16_t const array_layers,
        Format const format,
        TextureFlags const flags
    );

    void delete_texture(Handle<Texture> const& texture);

    Handle<Buffer> create_buffer(size_t const size, BufferFlags const flags);

    void delete_buffer(Handle<Buffer> const& buffer);

    Handle<RenderPass> create_render_pass(
        std::span<Handle<Texture>> const& colors,
        std::span<RenderPassColorDesc const> const& color_descs,
        Handle<Texture> const& depth_stencil,
        RenderPassDepthStencilDesc const& depth_stencil_desc
    );

    void delete_render_pass(Handle<RenderPass> const& render_pass);

    Handle<Sampler> create_sampler(
        Filter const filter,
        AddressMode const address_u,
        AddressMode const address_v,
        AddressMode const address_w,
        uint16_t const anisotropic,
        CompareOp const compare_op
    );

    void delete_sampler(Handle<Sampler> const& sampler);

    Handle<Shader> create_shader(std::span<uint8_t const> const data, ShaderFlags const flags);

    void delete_shader(Handle<Shader> const& shader);

    Handle<DescriptorLayout> create_descriptor_layout(std::span<DescriptorRangeDesc const> const ranges);

    void delete_descriptor_layout(Handle<DescriptorLayout> const& descriptor_layout);

    Handle<DescriptorSet> create_descriptor_set(Handle<DescriptorLayout> const& descriptor_layout);

    void delete_descriptor_set(Handle<DescriptorSet> const& descriptor_set);

    Handle<Pipeline> create_pipeline(
        Handle<DescriptorLayout> const& descriptor_layout,
        std::span<VertexInputDesc const> const vertex_descs,
        std::span<Handle<Shader> const> const shaders,
        RasterizerDesc const& rasterizer_desc,
        DepthStencilDesc const& depth_stencil_desc,
        BlendDesc const& blend_desc,
        Handle<RenderPass> const& render_pass,
        Handle<CachePipeline> const& cache_pipeline
    );

    void delete_pipeline(Handle<Pipeline> const& pipeline);

    Handle<CommandList> create_command_list(QueueFlags const flags);

    void delete_command_list(Handle<CommandList> const& command_list);

    void update_descriptor_set(Handle<DescriptorSet> const& descriptor_set, std::span<DescriptorWriteDesc const> const write_descs);

    void map_buffer_data(Handle<Buffer> const& buffer, uint64_t const offset, std::span<uint8_t const> const data);

    uint64_t resource_fence(ResourceHandle const& target) const;

    void present();

    Handle<Texture> acquire_next_texture();

    void recreate_swapchain(uint32_t const width, uint32_t const height, std::optional<SwapchainDesc> swapchain_desc = std::nullopt);

    uint64_t submit(std::span<Handle<CommandList> const> const command_lists, QueueFlags const flags);

    uint64_t submit_after(std::span<Handle<CommandList> const> const command_lists, uint64_t const fence_value, QueueFlags const flags);

    void wait(uint64_t const fence_value, QueueFlags const flags);

    bool is_completed(uint64_t const fence_value, QueueFlags const flags) const;

    void wait_for_idle(QueueFlags const flags);

    void copy_buffer_region(
        Handle<CommandList> const& command_list,
        Handle<Buffer> const& dest, 
        uint64_t const dest_offset, 
        Handle<Buffer> const& source, 
        uint64_t const source_offset,
        size_t const size
    );

    void copy_texture_region();

    void bind_vertex_buffer(Handle<CommandList> const& command_list, uint32_t const index, Handle<Buffer> const& buffer, uint64_t const offset);

    void bind_index_buffer(Handle<CommandList> const& command_list, Handle<Buffer> const& buffer, uint64_t const offset);

    void barrier(Handle<CommandList> const& command_list, ResourceHandle const& target, MemoryState const before, MemoryState const after);

    void bind_pipeline(Handle<CommandList> const& command_list, Handle<Pipeline> const& pipeline);

    void bind_descriptor_set(Handle<CommandList> const& command_list, Handle<DescriptorSet> const& descriptor_set);

    void set_viewport(Handle<CommandList> const& command_list, uint32_t const x, uint32_t const y, uint32_t const width, uint32_t const height);

    void set_scissor(Handle<CommandList> const& command_list, uint32_t const left, uint32_t const top, uint32_t const right, uint32_t const bottom);

    void begin_render_pass(
        Handle<CommandList> const& command_list,
        Handle<RenderPass> const& render_pass, 
        std::span<Color const> const clear_colors, 
        float const clear_depth = 0.0f,
        uint8_t const clear_stencil = 0x0
    );

    void end_render_pass(Handle<CommandList> const& command_list);

    void draw(Handle<CommandList> const& command_list, uint32_t const vertex_count, uint32_t const instance_count, uint32_t const vertex_offset);

    void draw_indexed(Handle<CommandList> const& command_list, uint32_t const index_count, uint32_t const instance_count, uint32_t const instance_offset);

private:

    struct Impl;
    struct impl_deleter { void operator()(Impl* ptr) const; };
    std::unique_ptr<Impl, impl_deleter> _impl;
};

}
