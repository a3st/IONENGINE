// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

    rpasses.resize(2);
}

void WorldRenderer::update() {

    Handle<Texture> texture = _backend->begin_frame();

    if(rpasses[frame_index] == Handle<RenderPass>()) {
        rpasses[frame_index] = _backend->create_render_pass(
            { texture },
            { RenderPassColorDesc { RenderPassLoadOp::Clear, RenderPassStoreOp::Store } },
            {}, {}
        );
    }

    _backend->set_viewport(0, 0, 800, 600);
    _backend->set_scissor(0, 0, 800, 600);
    _backend->barrier(texture, MemoryState::Present, MemoryState::RenderTarget);
    _backend->begin_render_pass(rpasses[frame_index], { Color(0.2f, 0.1f, 0.3f, 1.0f) }, {});
    _backend->end_render_pass();
    _backend->barrier(texture, MemoryState::RenderTarget, MemoryState::Present);

    _backend->end_frame();

    frame_index = (frame_index + 1) % 2;

    std::cout << "frame end" << std::endl;
}