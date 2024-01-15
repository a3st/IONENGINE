// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "upload_manager.hpp"
#include "backend.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

UploadManager::UploadManager(Backend& backend) : backend(&backend) {

    upload_buffer = core::make_ref<Buffer>(
        backend,
        16 * 1024 * 1024,
        wgpu::BufferUsage::MapWrite | wgpu::BufferUsage::CopySrc
    );
}