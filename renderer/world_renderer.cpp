// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend) : 
    backend_(backend) {

    buffer = Buffer(*backend_, BufferType::Vertex, 1024 * 1024);
    memory = Memory(*backend_, MemoryType::Upload, 1024 * 1024 * 512);
    memory.bind_buffer(buffer, 0);
    char8_t* data = memory.map();
    memset(data, 1, sizeof(int));
    memory.unmap();
}

void WorldRenderer::update() {

    
}