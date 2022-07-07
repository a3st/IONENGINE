// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/color.h>
#include <platform/window.h>
#include <renderer/backend/handle.h>

namespace ionengine::renderer::backend {

struct Buffer;
struct Texture;
struct Pipeline;
struct Shader;
struct RenderPass;
struct Sampler;
struct DescriptorLayout;
struct CachePipeline;
struct CommandList;

///
/// Backend limits describes limit of using resources.
/// It is ideal to choose the values ​​for each platform, but here the
/// optimal ones are selected.
///
enum class BackendLimits : uint32_t {
    Textures = 8192,
    Buffers = 8192,
    Samplers = 8192,
    DescriptorLayouts = 256,
    Pipelines = 256,
    Shaders = 1024,
    RenderPasses = 32,
    CommandLists = 512
};

///
/// Render pass color attachment count limit per one pass
///
inline uint32_t constexpr RENDER_PASS_COLOR_ATTACHMENT_COUNT = 8;

///
/// Maximum number of back buffers for swap chain
///
inline uint32_t constexpr BACK_BUFFER_COUNT = 2;

/// 
/// Texture row alignment value that are optimal for Direct3D12 and Vulkan
///
inline uint32_t constexpr TEXTURE_ROW_PITCH_ALIGNMENT = 256;

/// 
/// Resource alignment value that are optimal for Direct3D12 and Vulkan
///
inline uint32_t constexpr TEXTURE_RESOURCE_ALIGNMENT = 512;

///
/// Available texture dimensions
///
enum class Dimension { _1D, _2D, _3D, Cube };

///
/// Available formats for resources
///
enum class Format {
    Unknown,
    BGRA8_UNORM,
    BGR8_UNORM,
    RGBA8_UNORM,
    R8_UNORM,
    RGBA32_FLOAT,
    RGB32_FLOAT,
    RG32_FLOAT,
    R32_FLOAT,
    BC1_UNORM,
    BC3_UNORM,
    BC4_UNORM,
    BC5_UNORM,
    D32_FLOAT,
    RGBA16_FLOAT
};

///
/// Available texture flags
///
enum class TextureFlags : uint16_t {
    ShaderResource = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
    UnorderedAccess = 1 << 3,
    HostWrite = 1 << 4,
    HostRead = 1 << 5
};

DECLARE_ENUM_CLASS_BIT_FLAG(TextureFlags)

///
/// Available buffer flags
///
enum class BufferFlags : uint16_t {
    ConstantBuffer = 1 << 0,
    UnorderedAccess = 1 << 1,
    ShaderResource = 1 << 2,
    HostWrite = 1 << 3,
    HostRead = 1 << 4
};

DECLARE_ENUM_CLASS_BIT_FLAG(BufferFlags)

///
/// Available command list flags
///
enum class CommandListFlags : uint16_t {
    Graphics = 1 << 0,
    Copy = 1 << 1,
    Compute = 1 << 2,
    Primary = 1 << 3,
    Secondary = 1 << 4
};

DECLARE_ENUM_CLASS_BIT_FLAG(CommandListFlags)

///
/// Available shader flags
///
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

///
/// Available queue flags
///
enum class QueueFlags : uint16_t {
    Graphics = 1 << 0,
    Copy = 1 << 1,
    Compute = 1 << 2
};

DECLARE_ENUM_CLASS_BIT_FLAG(QueueFlags)

///
/// Render pass load operations
///
enum class RenderPassLoadOp { Load, Clear, DontCare };

///
/// Render pass store operations
///
enum class RenderPassStoreOp { Store, DontCare };

///
/// Available memory states
///
enum class MemoryState : uint32_t {
    Common = 1 << 0,
    Present = 1 << 1,
    RenderTarget = 1 << 2,
    UnorderedAccess = 1 << 3,
    DepthWrite = 1 << 4,
    CopySource = 1 << 5,
    CopyDest = 1 << 6,
    ResolveSource = 1 << 7,
    ResolveDest = 1 << 8,
    ShaderRead = 1 << 9,
    DepthRead = 1 << 10,
    VertexConstantBufferRead = 1 << 11,
    IndexBufferRead = 1 << 12,
    GenericRead = IndexBufferRead | VertexConstantBufferRead | DepthRead | ShaderRead
};

DECLARE_ENUM_CLASS_BIT_FLAG(MemoryState)

///
/// Available descriptor types
///
enum class DescriptorType {
    ShaderResource,
    ConstantBuffer,
    UnorderedAccess,
    Sampler
};

///
/// Available rasterizer fill modes
///
enum class FillMode { Wireframe, Solid };

///
/// Available rasterizer cull modes
///
enum class CullMode { None, Front, Back };

///
/// Available compare operations
///
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

///
/// Available blends
///
enum class Blend { Zero, One, SrcAlpha, InvSrcAlpha, BlendFactor };

///
/// Available blend operations
///
enum class BlendOp { Add, Subtract, RevSubtract, Min, Max };

///
/// Available filters
///
enum class Filter { Anisotropic, MinMagMipLinear, ComparisonMinMagMipLinear };

///
/// Available address modes
///
enum class AddressMode { Wrap, Clamp, Mirror };

///
/// Render pass color description
///
struct RenderPassColorDesc {
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
};

///
/// Render pass depth stencil description
///
struct RenderPassDepthStencilDesc {
    RenderPassLoadOp depth_load_op;
    RenderPassStoreOp depth_store_op;
    RenderPassLoadOp stencil_load_op;
    RenderPassStoreOp stencil_store_op;
};

///
/// Descriptor layout binding description
///
struct DescriptorLayoutBindingDesc {
    uint32_t index;
    DescriptorType type;
    ShaderFlags flags;
};

///
/// Descriptor write description
///
struct DescriptorWriteDesc {
    uint32_t index;
    std::variant<Handle<Texture>, Handle<Buffer>, Handle<Sampler>> data;
};

///
/// Pipeline vertex input description
///
struct VertexInputDesc {
    std::string semantic;
    uint32_t index;
    Format format;
    uint32_t slot;
    uint32_t stride;
};

///
/// Pipeline rasterizer description
///
struct RasterizerDesc {
    FillMode fill_mode;
    CullMode cull_mode;
};

///
/// Pipeline depth stencil description
///
struct DepthStencilDesc {
    CompareOp depth_func;
    bool depth_write;
    bool stencil_write;
};

///
/// Pipeline blend description
///
struct BlendDesc {
    bool blend_enable;
    Blend blend_src;
    Blend blend_dst;
    BlendOp blend_op;
    Blend blend_src_alpha;
    Blend blend_dst_alpha;
    BlendOp blend_op_alpha;
};

///
/// Texture copy region description
///
struct TextureCopyRegionDesc {
    uint32_t mip_index;
    uint32_t row_pitch;
    uint64_t offset;
};

///
/// Memory barrier description
///
struct MemoryBarrierDesc {
    std::variant<Handle<Texture>, Handle<Buffer>> resource;
    MemoryState before;
    MemoryState after;
};

///
/// GPU adapter description
///
struct GPUAdapterDesc {
    std::string name;
    size_t local_memory_size;
    size_t local_memory_usage;
    bool is_uma;
    // TODO
};

class Device {
 public:
    ///
    /// Create new graphics device
    /// @param adapter_index Adapter index
    /// @param window Swap chain target window
    /// @param sample_count Swap chain sample count
    ///
    Device(uint32_t const adapter_index, platform::Window& window, uint16_t const sample_count);

    ~Device();

    Device(Device const&) = delete;

    Device(Device&&) noexcept = delete;

    Device& operator=(Device const&) = delete;

    Device& operator=(Device&&) noexcept = delete;

    ///
    /// Create new texture on graphics device
    /// @param dimension Dimension of texture
    /// @param width Width of texture
    /// @param height Height of texture
    /// @param mip_levels Mip levels of texture
    /// @param array_layers Array levels of texture (3D/Cube dimension), or default it should be 1
    /// @param format Format of texture
    /// @param flags Flags of texture
    /// @return Texture handle
    ///
    Handle<Texture> create_texture(Dimension const dimension,
                                   uint32_t const width, uint32_t const height,
                                   uint16_t const mip_levels,
                                   uint16_t const array_layers,
                                   Format const format,
                                   TextureFlags const flags);

    ///
    /// Delete texture on graphics device
    /// @param texture Texture handle
    ///
    void delete_texture(Handle<Texture> const& texture);

    ///
    /// Create buffer on graphics device
    /// @param size Size in bytes
    /// @param stride Stride element in bytes for structure buffers
    /// @param flags Flags of buffer
    /// @return Buffer handle
    ///
    Handle<Buffer> create_buffer(size_t const size, uint32_t const stride, BufferFlags const flags);

    ///
    /// Delete buffer on graphics device
    /// @param buffer Buffer handle
    ///
    void delete_buffer(Handle<Buffer> const& buffer);

    ///
    /// Create render pass on graphics device
    /// @param colors Array of color texture handles
    /// @param color_descs Array of color descriptions
    /// @param depth_stencil Depth stencil texture handle
    /// @param depth_stencil_desc Depth stencil description
    /// @return Render pass handle
    ///
    Handle<RenderPass> create_render_pass(
        std::span<Handle<Texture> const> const& colors,
        std::span<RenderPassColorDesc const> const& color_descs,
        Handle<Texture> const& depth_stencil,
        std::optional<RenderPassDepthStencilDesc> const& depth_stencil_desc);

    ///
    /// Delete render pass on graphics device
    /// @param render_pass Render pass handle
    ///
    void delete_render_pass(Handle<RenderPass> const& render_pass);

    ///
    /// Create sampler on graphics device
    /// @param filter Sampler filter
    /// @param addresses Array of addresses [u, v, w]
    /// @param anisotropic Anisotropic count
    /// @param compare_op Compare operation
    /// @return Sampler handle
    ///
    Handle<Sampler> create_sampler(Filter const filter,
                                   std::array<AddressMode, 3> const& addresses,
                                   uint16_t const anisotropic,
                                   CompareOp const compare_op);

    ///
    /// Delete sampler on graphics device
    /// @param sampler Sampler handle
    ///
    void delete_sampler(Handle<Sampler> const& sampler);

    ///
    /// Create shader on graphics device
    /// @param data Array of bytes SPIRV or DXIL (Backend supports only compiled shader code)
    /// @param flags Flags of shader
    /// @return Shader handle
    ///
    Handle<Shader> create_shader(std::span<uint8_t const> const data, ShaderFlags const flags);

    ///
    /// Delete shader on graphics device
    /// @param shader Shader handle
    ///
    void delete_shader(Handle<Shader> const& shader);

    /// 
    /// Create descriptor layout on graphics device
    /// @param binding_descs Array of binding descriptions
    /// @return Descriptor Layout handle
    ///
    Handle<DescriptorLayout> create_descriptor_layout(
        std::span<DescriptorLayoutBindingDesc const> const binding_descs);

    ///
    /// Delete descriptor layout on graphics device
    /// @param descriptor_layout Descriptor Layout handle
    ///
    void delete_descriptor_layout(
        Handle<DescriptorLayout> const& descriptor_layout);

    ///
    /// Create graphics pipeline on graphics device
    /// @param descriptor_layout Desciptor Layout handle
    /// @param vertex_input_descs Array of vertex input descriptions
    /// @param shaders Array of shader handles
    /// @param rasterizer_desc Rasterizer description
    /// @param depth_stencil_desc Depth stencil description
    /// @param blend_desc Blend description
    /// @param render_pass Render pass handle
    /// @param old_pipeline Old pipeline handle (For caching)
    /// @return Pipeline handle
    ///
    Handle<Pipeline> create_pipeline(
        Handle<DescriptorLayout> const& descriptor_layout,
        std::span<VertexInputDesc const> const vertex_input_descs,
        std::span<Handle<Shader> const> const shaders,
        RasterizerDesc const& rasterizer_desc,
        DepthStencilDesc const& depth_stencil_desc, BlendDesc const& blend_desc,
        Handle<RenderPass> const& render_pass,
        Handle<Pipeline> const& old_pipeline);

    ///
    /// Create compute pipeline on graphics device
    /// @param descriptor_layout Desciptor Layout handle
    /// @param shader Shader handle
    /// @param old_pipeline Old pipeline handle (For caching)
    /// @return Pipeline handle
    ///
    Handle<Pipeline> create_pipeline(
        Handle<DescriptorLayout> const& descriptor_layout,
        Handle<Shader> const& shader,
        Handle<Pipeline> const& old_pipeline);

    ///
    /// Delete pipeline on graphics device
    /// @param pipeline Pipeline handle
    ///
    void delete_pipeline(Handle<Pipeline> const& pipeline);

    ///
    /// Create command list on graphics device
    /// @param flags Flags of command list
    /// @return Command list handle
    ///
    Handle<CommandList> create_command_list(CommandListFlags const flags);

    ///
    /// Delete command list on graphics device
    /// @param command_list Command list handle
    ///
    void delete_command_list(Handle<CommandList> const& command_list);

    uint8_t* map_buffer_data(Handle<Buffer> const& buffer,
                             uint64_t const offset = 0);

    void unmap_buffer_data(Handle<Buffer> const& buffer);

    void present();

    uint32_t acquire_next_swapchain_texture();

    Handle<Texture> swapchain_texture(uint32_t const index);

    void recreate_swapchain(
        uint32_t const width, uint32_t const height);

    uint64_t queue_submit(std::span<Handle<CommandList> const> const command_lists, QueueFlags const flags);

    void queue_wait(QueueFlags const flags, uint64_t const fence_value);

    bool is_queue_completed(uint64_t const fence_value, QueueFlags const flags) const;

    void wait_for_queue_idle(QueueFlags const flags);

    void copy_buffer_region(Handle<CommandList> const& command_list,
                            Handle<Buffer> const& dest,
                            uint64_t const dest_offset,
                            Handle<Buffer> const& source,
                            uint64_t const source_offset, size_t const size);

    void copy_texture_region(Handle<CommandList> const& command_list,
                             Handle<Texture> const& dest,
                             Handle<Buffer> const& source,
                             std::span<TextureCopyRegionDesc const> const regions);

    void bind_vertex_buffer(Handle<CommandList> const& command_list,
                            uint32_t const index, Handle<Buffer> const& buffer,
                            uint64_t const offset);

    void bind_index_buffer(Handle<CommandList> const& command_list,
                           Handle<Buffer> const& buffer, uint64_t const offset);

    void barrier(Handle<CommandList> const& command_list,
                 std::span<MemoryBarrierDesc const> const barriers);

    void bind_pipeline(Handle<CommandList> const& command_list,
                       Handle<Pipeline> const& pipeline);

    void bind_resources(Handle<CommandList> const& command_list,
                        Handle<DescriptorLayout> const& descriptor_layout,
                        std::span<DescriptorWriteDesc const> const write_descs);

    void set_viewport(Handle<CommandList> const& command_list, int32_t const x,
                      int32_t const y, uint32_t const width,
                      uint32_t const height);

    void set_scissor(Handle<CommandList> const& command_list,
                     int32_t const left, int32_t const top, int32_t const right,
                     int32_t const bottom);

    void begin_render_pass(Handle<CommandList> const& command_list,
                           Handle<RenderPass> const& render_pass,
                           std::span<lib::math::Color const> const clear_colors,
                           float const clear_depth = 0.0f,
                           uint8_t const clear_stencil = 0x0);

    void end_render_pass(Handle<CommandList> const& command_list);

    void begin_command_list(Handle<CommandList> const& command_list);

    void close_command_list(Handle<CommandList> const& command_list);

    void execute_bundle(Handle<CommandList> const& command_list,
                        Handle<CommandList> const& bundle_command_list);

    void dispatch(Handle<CommandList> const& command_list,
                  uint32_t const thread_group_x, uint32_t const thread_group_y,
                  uint32_t const thread_group_z);

    void draw(Handle<CommandList> const& command_list,
              uint32_t const vertex_count, uint32_t const instance_count,
              uint32_t const vertex_offset);

    void draw_indexed(Handle<CommandList> const& command_list,
                      uint32_t const index_count, uint32_t const instance_count,
                      uint32_t const instance_offset);

    GPUAdapterDesc gpu_adapter_desc() const;

 private:
    struct Impl;
    struct impl_deleter {
        void operator()(Impl* ptr) const;
    };
    std::unique_ptr<Impl, impl_deleter> _impl;
};

}  // namespace ionengine::renderer::backend
