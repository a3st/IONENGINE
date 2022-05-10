// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache_ptr.h>
#include <renderer/shader_cache.h>
#include <renderer/geometry_cache.h>
#include <renderer/upload_context.h>
#include <renderer/backend/backend.h>
#include <asset/technique.h>
#include <asset/mesh.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

class CacheManager {
public:

    CacheManager(backend::Device& device, UploadContext& upload_context);

    CachePtr<ShaderProgram> get_shader_program(asset::Technique& technique);

    CachePtr<GeometryBuffer> get_geometry_buffer(asset::Surface& surface);

    void update(float const delta_time);

private:

    backend::Device* _device;
    UploadContext* _upload_context;

    CachePool<ShaderProgram> _shader_program_pool;
    CachePool<GeometryBuffer> _geometry_buffer_pool;

    /*struct UploadBuffer {
        backend::Handle<backend::Buffer> buffer;
        uint64_t offset;
    };

    size_t _buffer_size = 1024 * 1024 * 16;

    std::vector<backend::Handle<backend::CommandList>> _command_lists;
    std::vector<UploadBuffer> _buffers;

    uint32_t _frame_index{0};
    uint32_t _frame_count{0};*/


};

}