// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_context.h>

using namespace ionengine;
using namespace ionengine::renderer;

UploadContext::UploadContext(backend::Device& device, uint32_t const frame_index) {

}

void UploadContext::upload(CachePtr<GeometryBuffer> resource, std::span<uint8_t const> const vertices, std::span<uint8_t const> const indices) {

}