// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache_ptr.h>
#include <renderer/geometry_buffer.h>

namespace ionengine::renderer {

class UploadContext {
public:

    UploadContext(backend::Device& device, uint32_t const frame_index);

    void upload(CachePtr<GeometryBuffer> resource, std::span<uint8_t const> const vertices, std::span<uint8_t const> const indices);

private:


};

}