// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/render_queue.h>
#include <renderer/upload_manager.h>
#include <renderer/shader_cache.h>
#include <renderer/pipeline_cache.h>
#include <renderer/frame_graph.h>
#include <renderer/mesh_renderer.h>
#include <renderer/ui_renderer.h>
#include <asset/asset_manager.h>
#include <lib/thread_pool.h>

namespace ionengine {

namespace scene {
class Scene;
}

namespace ui {
class UserInterface;
};

}

namespace ionengine::renderer {

class Renderer {
public:

    Renderer(platform::Window& window, asset::AssetManager& asset_manager, lib::ThreadPool& thread_pool);

    ~Renderer();

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void update(float const delta_time);

    void render(scene::Scene& scene, ui::UserInterface& ui);

    void resize(uint32_t const width, uint32_t const height);

    UiRenderer& ui_renderer();

    MeshRenderer& mesh_renderer();

private:

    backend::Device _device;

    UploadManager _upload_manager;
    FrameGraph _frame_graph;

    UiRenderer _ui_renderer;
    MeshRenderer _mesh_renderer;

    ShaderCache _shader_cache;
    PipelineCache _pipeline_cache;

    uint32_t _width;
    uint32_t _height;
};

}
