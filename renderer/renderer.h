// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/render_queue.h>
#include <renderer/frontend/frame_graph.h>
#include <renderer/cache_manager.h>
#include <asset/asset_manager.h>

namespace ionengine {

namespace scene {
class Scene;
}

}

namespace ionengine::renderer {

class Renderer {
public:

    Renderer(platform::Window& window, asset::AssetManager& asset_manager);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void update(float const delta_time);

    void render(scene::Scene& scene);

    void resize(uint32_t const width, uint32_t const height);

private:

    asset::AssetManager* _asset_manager;

    backend::Device _device;

    UploadContext _upload_context;

    std::optional<lib::Receiver<asset::AssetEvent<asset::Mesh>>> _mesh_event_receiver;
    std::optional<lib::Receiver<asset::AssetEvent<asset::Technique>>> _technique_event_receiver;

    std::optional<frontend::FrameGraph> _frame_graph;
    std::optional<CacheManager> _cache_manager;

    frontend::Attachment* _gbuffer_color_buffer;
    frontend::Attachment* _swapchain_buffer;

    uint32_t _frame_count{0};
    uint32_t _frame_index{0};

    std::vector<uint64_t> _graphics_fence_values;

    RenderQueue _deffered_queue;

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count);
    
    void swap_buffers();
    
    backend::Handle<backend::Texture> get_or_wait_previous_buffer();

    // TEST ONLY
    std::vector<backend::VertexInputDesc> vertex_declaration;
    std::unordered_map<uint32_t, backend::Handle<backend::Pipeline>> _pipelines;
    backend::Handle<backend::Sampler> _sampler;
    std::vector<backend::Handle<backend::Buffer>> _model_buffer;
};

}