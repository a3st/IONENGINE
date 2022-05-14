// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/render_queue.h>
#include <renderer/upload_context.h>
#include <renderer/shader_cache.h>
#include <renderer/geometry_cache.h>
#include <renderer/gpu_texture.h>
#include <renderer/frame_graph.h>
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

    std::optional<lib::Receiver<asset::AssetEvent<asset::Mesh>>> _mesh_event_receiver;
    std::optional<lib::Receiver<asset::AssetEvent<asset::Technique>>> _technique_event_receiver;

    std::optional<UploadContext> _upload_context;
    std::optional<ShaderCache> _shader_cache;
    std::optional<GeometryCache> _geometry_cache;
    std::optional<FrameGraph> _frame_graph;

    std::shared_ptr<GPUTexture> _gbuffer_albedo;

    RenderQueue _deffered_queue;

    uint32_t _width;
    uint32_t _height;

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const frame_index);

    // TEST ONLY
    std::vector<backend::VertexInputDesc> vertex_declaration;
    std::unordered_map<uint32_t, backend::Handle<backend::Pipeline>> _pipelines;
    backend::Handle<backend::Sampler> _sampler;
    std::vector<backend::Handle<backend::Buffer>> _model_buffer;
};

}