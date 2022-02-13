// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
//#include <renderer/frame_graph.h>
#include <lib/thread_pool.h>

namespace ionengine::renderer {

class WorldRenderer {
public:

    WorldRenderer(Backend* const backend, ThreadPool* const thread_pool);

    WorldRenderer(WorldRenderer const&) = delete;

    WorldRenderer(WorldRenderer&&) = delete;

    WorldRenderer& operator=(WorldRenderer const&) = delete;

    WorldRenderer& operator=(WorldRenderer&&) = delete;

    void update();

private:

    Backend* _backend;
    ThreadPool* _thread_pool;

    //FrameGraph _frame_graph;

    std::vector<Handle<RenderPass>> rpasses;
    std::vector<Handle<Pipeline>> pipelines;
    std::vector<Handle<Shader>> shaders;
    Handle<DescriptorLayout> desc_layout;
    Handle<Buffer> buffer_vertex;
    Handle<Buffer> constant_buffer;
    uint32_t frame_index{0};
};

}