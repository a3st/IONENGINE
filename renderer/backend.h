// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/render_queue.h>
#include <platform/window.h>

namespace ionengine::renderer {

class GPUResourceHandle {
public:

    GPUResourceHandle() = default;
    GPUResourceHandle(uint32_t const id) : _id(id) { }

private:
    
    uint32_t _id;
};

enum class TextureDimension {
    _1D,
    _2D,
    _3D,
    Cube
};

enum class TextureFormat {
    Unknown,
    RGBA8Unorm
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

enum class BarrierType {
    Common,
    Copy
};

enum class PipelineType {
    Graphics,
    Compute
};

struct BatchInfo {
    uint32_t primitives_count;
};

struct ComputeInfo {
    std::array<uint32_t, 3> thread_count;
    bool async;
};

struct PipelineContext {
    std::vector<ImageViewId> targets; // render to (RTV)
    // std::vector<ClearColor> clear_colors; - render pass emulation
    // std::vector<ShaderParam> params; - binding shader params.
    // ShaderParam
    BufferViewId vertex_buffer;
    BufferViewId index_buffer;
};

struct RenderContext {
    BatchInfo batch_info;
    ComputeInfo compute_info;
    PipelineId pipeline_id;
};

struct Extent2D {
    uint32_t width;
    uint32_t height;
};

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
        TextureFormat const format,
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

    GPUResourceHandle generate_command_buffer(RenderQueue const& queue);

    void execute_command_buffers(std::vector<GPUResourceHandle> const& handles);

    void swap_buffers();

    void resize_buffers(uint32_t const width, uint32_t const height, uint32_t frame_count);

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}