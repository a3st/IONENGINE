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

    std::vector<GPUResourceHandle> cmdbuffs;
};

}