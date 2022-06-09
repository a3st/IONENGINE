// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/resource_ptr.h>
#include <renderer/gpu_texture.h>
#include <renderer/gpu_buffer.h>
#include <renderer/geometry_buffer.h>
#include <lib/thread_pool.h>

namespace ionengine::renderer {

inline uint32_t constexpr UPLOAD_MANAGER_BUFFER_COUNT = 2;

class UploadManager {
public:

    UploadManager(lib::ThreadPool& thread_pool, backend::Device& device);

    void upload_texture_data(ResourcePtr<GPUTexture> resource, std::span<uint8_t const> const data, bool const async = true);

    void upload_buffer_data(ResourcePtr<GPUBuffer> resource, uint64_t const offset, std::span<uint8_t const> const data, bool const async = true);

    void upload_geometry_data(ResourcePtr<GeometryBuffer> resource, std::span<uint8_t const> const vertex_data, std::span<uint8_t const> const index_data, bool const async = true);

    void update();

private:

    template<class Type>
    struct UploadData {
        ResourcePtr<Type> ptr;
        Type resource;
    };

    struct UploadBuffer {
        backend::Handle<backend::CommandList> command_list;
        backend::Handle<backend::Buffer> buffer;
        uint32_t size;
        uint64_t offset;
        bool is_close;
    };

    backend::Device* _device;
    lib::ThreadPool* _thread_pool;

    std::array<UploadBuffer, UPLOAD_MANAGER_BUFFER_COUNT> _upload_buffers;
    uint32_t _buffer_index{0};

    std::array<uint64_t, UPLOAD_MANAGER_BUFFER_COUNT> _fence_values;

    std::mutex _mutex;

    std::array<std::queue<UploadData<GPUTexture>>, UPLOAD_MANAGER_BUFFER_COUNT> _textures;
    std::array<std::queue<UploadData<GPUBuffer>>, UPLOAD_MANAGER_BUFFER_COUNT> _buffers;
    std::array<std::queue<UploadData<GeometryBuffer>>, UPLOAD_MANAGER_BUFFER_COUNT> _geometry_buffers;
};

}
