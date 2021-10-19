// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "renderer.h"

using namespace ionengine::rendersystem;

Renderer::Renderer(platform::Window* window) : window_(window) {

    device_ = lgfx::Device(0, window->GetNativeHandle(), 800, 600, 2, 1);
    mem_pool = lgfx::MemoryPool(&device_, 1024, 0, lgfx::MemoryType::kDefault, lgfx::MemoryFlags::kBuffers);

    //lgfx::Device dev = std::move(device_);

    //device_ = std::move(dev);
}

void Renderer::Frame() {

    
}