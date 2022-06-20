// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/command_pool.h>
#include <renderer/gpu_texture.h>
#include <renderer/gpu_buffer.h>
#include <renderer/geometry_buffer.h>
#include <lib/thread_pool.h>

namespace ionengine::renderer {

inline size_t constexpr UPLOAD_MANAGER_BUFFER_SIZE = 32 * 1024 * 1024;
inline uint32_t constexpr UPLOAD_MANAGER_DISPATCH_TASK_COUNT = 64;

template<class Type>
struct UploadBatchData { };

template<>
struct UploadBatchData<GeometryBuffer> {
    ResourcePtr<GeometryBuffer> geometry_buffer;
    asset::AssetPtr<asset::Mesh> asset;
    uint32_t surface_index;
};

template<>
struct UploadBatchData<GPUTexture> {
    ResourcePtr<GPUTexture> texture;
    asset::AssetPtr<asset::Texture> asset;
};

struct UploadBatch {
    std::variant<
        UploadBatchData<GeometryBuffer>,
        UploadBatchData<GPUTexture>
    > data;
};

template<class Type>
struct UploadTaskData { 
    ResourcePtr<Type> ptr;
    Type resource;
};

struct UploadTask {
    std::variant<
        UploadTaskData<GPUBuffer>,
        UploadTaskData<GPUTexture>,
        UploadTaskData<GeometryBuffer>
    > data;

    ResourcePtr<CommandList> command_list;
};

struct UploadBuffer {
    backend::Handle<backend::Buffer> buffer;
    size_t size;
    uint64_t offset;
};

class UploadManager {
public:

    UploadManager(backend::Device& device);

    bool upload_texture_data(ResourcePtr<GPUTexture> resource, std::span<uint8_t const> const data, bool const async = true);

    bool upload_buffer_data(ResourcePtr<GPUBuffer> resource, uint64_t const offset, std::span<uint8_t const> const data, bool const async = true);

    bool upload_geometry_data(ResourcePtr<GeometryBuffer> resource, std::span<uint8_t const> const vertex_data, std::span<uint8_t const> const index_data, bool const async = true);

    void dispatch();

private:

    backend::Device* _device;

    std::unique_ptr<UploadBuffer> _upload_buffer;

    CommandPool<CommandPoolType::Copy> _copy_command_pool;

    uint64_t _fence_value;

    std::vector<UploadTask> _tasks;

    std::atomic<uint32_t> _pending_upload_task_count;
    uint32_t _dispatch_upload_task_count;

    void get_texture_data(backend::Format const format, uint32_t const width, uint32_t const height, size_t& row_bytes, uint32_t& row_count);

    bool get_required_memory_offset(size_t const size, uint64_t& required_offset);
};

}
