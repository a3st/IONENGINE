// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifndef HELPER_DEFINE_HANDLE
#define HELPER_DEFINE_HANDLE(Name) \
class Name { \
public: \
    Name() { } \
    Name(uint64_t const id) : _id(id) { } \
    uint64_t id() const { return _id; } \
private: \
    uint64_t _id; \
};
#endif

namespace ionengine::platform { class Window; }

namespace ionengine::renderer {

enum class ImageDimension {
    _1D,
    _2D,
    _3D
};

enum class ImageFormat {
    Unknown,
    RGBA8Unorm
};

enum class ImageFlags {
    Color,
    DepthStencil,
    Sampled
};

enum class BufferFlags {
    None,
    Vertex,
    Index,
    Constant,
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

struct BatchInfo {
    uint32_t primitives_count;
};

struct ComputeInfo {
    std::array<uint32_t, 3> thread_count;
    bool async;
};

struct ShaderContext {
    std::vector<ImageViewId> targets; // render to (RTV)
    // std::vector<ClearColor> clear_colors; - render pass emulation
    // std::vector<ShaderParam> params; - binding shader params.
    // ShaderParam
};

struct RenderJob {
    BatchInfo batch_info;
    ComputeInfo compute_info;
    ShaderId shader_id;
};

HELPER_DEFINE_HANDLE(BufferId)
HELPER_DEFINE_HANDLE(BufferViewId)
HELPER_DEFINE_HANDLE(ImageId)
HELPER_DEFINE_HANDLE(ImageViewId)
HELPER_DEFINE_HANDLE(ShaderId)

class Backend {
public:

    Backend(uint32_t const adapter_index, platform::Window* const window, uint32_t const frame_count);

    ~Backend();

    Backend(Backend const&) = delete;

    Backend(Backend&&) = delete;

    Backend& operator=(Backend const&) = delete;

    Backend& operator=(Backend&&) = delete;

    ImageId create_image(
        ImageDimension const dimension, 
        uint32_t const width, 
        uint32_t const height, 
        uint16_t const mip_levels, 
        uint16_t const array_layers,
        ImageFormat const format,
        ImageFlags const flags
    );

    void free_image(ImageId const& image_id);

    ImageViewId create_image_view(
        ImageId const& image_id,
        ImageDimension const dimension,
        uint16_t const base_mip_level, 
        uint16_t const mip_level_count,
        uint16_t const base_array_layer, 
        uint16_t const array_layer_count
    );

    void free_image_view(ImageViewId const& image_view_id);

    BufferId create_buffer(
        size_t const size,
        BufferFlags const flags
    );

    void free_buffer(BufferId const& buffer_id);

    BufferViewId create_buffer_view(
        BufferId const& buffer_id,
        uint32_t const stride
    );

    void free_buffer_view(BufferViewId const& buffer_view_id);

    ShaderId create_shader();

    void free_shader(ShaderId const& shader_id);

    void barrier(BufferId const& buffer_id, BarrierType const barrier_type, uint64_t const sort_key);

    void render(RenderJob const& job, ShaderContext const& shader_context, uint64_t const sort_key);

    // void copy(, uint64_t const sort_key);

    void wait_fence(std::string const& fence_name, uint64_t const sort_key, QueueType const queue_type);

    void signal_fence(std::string const& fence_name, uint64_t const sort_key, QueueType const queue_type);

    void dispatch();

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}