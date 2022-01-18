// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/window.h>
#include <renderer/color.h>

namespace ionengine::renderer {

enum class GPUResourceType : uint16_t {
    Texture = 0x1,
    Buffer = 0x2,
    CommandBuffer = 0x3,
    Pipeline = 0x4
};

class GPUResourceHandle {
public:

    GPUResourceHandle() = default;
    
    GPUResourceHandle(GPUResourceType const res_type, uint16_t const id) { 

        _id = (static_cast<uint32_t>(res_type) << 16) + id; 
    }

    bool operator==(GPUResourceHandle const& other) {
        return _id == other._id;
    }

    bool operator!=(GPUResourceHandle const& other) {
        return _id != other._id;
    }

private:
    
    uint32_t _id{std::numeric_limits<uint32_t>::max()};

    friend class CommandGenerator;
    friend class Backend;
};

enum class TextureDimension {
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

enum class TextureFlags {
    None,
    RenderTarget,
    DepthStencil,
    UnorderedAccess
};

enum class BufferFlags {
    None,
    Vertex,
    Index,
    ConstantBuffer,
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

enum class ShaderStageFlags : uint32_t {
    Vertex = 1 << 0,
    Geometry = 1 << 1,
    Domain = 1 << 2,
    Pixel = 1 << 3,
    Compute = 1 << 4,
    All = Vertex | Geometry | Domain | Pixel | Compute
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

struct ShaderBindDesc {
    ShaderBindType bind_type;
    uint32_t index;
    uint32_t count;
    ShaderStageFlags stage_flags;
};

struct VertexInputDesc {
    std::string semantic;
    uint32_t index;
    Format format;
    uint32_t slot;
    uint32_t stride;
};

struct ShaderStageDesc {
    std::span<char8_t> data;
    ShaderStageFlags stage_flags;
};

struct RasterizerDesc {
    FillMode fill_mode;
    CullMode cull_mode;
}

struct DepthStencilDesc {
    CompareOp depth_func;
    bool write_enable;
}

class Backend {
public:

    Backend(uint32_t const adapter_index, platform::Window* const window, uint32_t const frame_count);

    ~Backend();

    Backend(Backend const&) = delete;

    Backend(Backend&&) = delete;

    Backend& operator=(Backend const&) = delete;

    Backend& operator=(Backend&&) = delete;

    GPUResourceHandle create_texture(
        TextureDimension const dimension,
        Extent2D const extent,
        uint16_t const mip_levels,
        uint16_t const array_layers,
        Format const format,
        TextureFlags const flags
    );

    void destroy_texture(GPUResourceHandle const& handle);

    void create_shader_view(
        GPUResourceHandle const& handle,
        TextureDimension const dimension,
        uint16_t const base_mip_level, 
        uint16_t const mip_level_count,
        uint16_t const base_array_layer, 
        uint16_t const array_layer_count
    );

    GPUResourceHandle create_buffer(
        size_t const size,
        BufferFlags const flags
    );

    void destroy_buffer(GPUResourceHandle const& handle);

    void create_shader_view(
        GPUResourceHandle const& handle,
        uint16_t dummy
    );

    GPUResourceHandle create_pipeline(
        PipelineType const pipeline_type,
        std::vector<ShaderBindDesc> const& shader_bindings,
        std::vector<VertexInputDesc> const& vertex_inputs,
        std::vector<ShaderStageDesc> const& shader_stages,
        RasterizerDesc const& rasterizer,
        DepthStencilDesc const& depth_stencil,
        
    );

    GPUResourceHandle create_command_buffer(QueueType const queue_type);

    CommandGenerator create_command_generator(GPUResourceHandle const& handle);

    void execute_command_buffers(QueueType const queue_type, std::vector<GPUResourceHandle> const& handles);

    GPUResourceHandle get_swap_texture() const;

    void swap_buffers();

    void resize_buffers(uint32_t const width, uint32_t const height, uint32_t const buffer_count);

private:

    struct Impl;
    std::unique_ptr<Impl> _impl;

    friend class CommandGenerator;
};

class CommandGenerator {
public:

    CommandGenerator(Backend& backend, uint16_t const cmd_index);

    ~CommandGenerator();

    CommandGenerator& set_pipeline(GPUResourceHandle const& handle);

    CommandGenerator& set_viewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);

    CommandGenerator& set_scissor(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);

    CommandGenerator& barrier(GPUResourceHandle const& handle, MemoryState const before, MemoryState const after);
        
    CommandGenerator& render_pass(
        uint16_t const rtv_count,
        std::array<GPUResourceHandle, 8> const& rtv_handles,
        std::array<RenderPassColorDesc, 8> const& rtv_ops,
        std::array<Color, 8> const& rtv_clears,
        GPUResourceHandle dsv_handle,
        RenderPassDepthStencilDesc dsv_op,
        std::pair<float, uint8_t> dsv_clear,
        std::function<void()> const& func
    );

private:

    Backend* _backend;
    uint16_t _cmd_index{};
};

}