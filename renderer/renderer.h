// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/render_queue.h>
#include <renderer/upload_manager.h>
#include <renderer/shader_uniform_binder.h>
#include <renderer/shader_cache.h>
#include <renderer/geometry_cache.h>
#include <renderer/texture_cache.h>
#include <renderer/pipeline_cache.h>
#include <renderer/frame_graph.h>
#include <renderer/buffer_pool.h>
#include <asset/asset_manager.h>
#include <lib/thread_pool.h>

namespace ionengine {

namespace scene {
class Scene;
class CameraNode;
}

}

namespace ionengine::renderer {
    
struct RendererAssets {
    asset::AssetPtr<asset::Technique> deffered;
    asset::AssetPtr<asset::Technique> lighting;
    asset::AssetPtr<asset::Technique> billboard;
    asset::AssetPtr<asset::Mesh> quad;
};

struct EditorInstance {
    asset::AssetPtr<asset::Texture> icon;
    lib::math::Matrixf model;
};

class Renderer {
public:

    Renderer(platform::Window& window, asset::AssetManager& asset_manager, lib::ThreadPool& thread_pool, RendererAssets const& assets);

    ~Renderer();

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void update(float const delta_time);

    void render(scene::Scene& scene);

    void resize(uint32_t const width, uint32_t const height);

    void editor_mode(bool const enable);

private:

    struct GBufferData {
        ResourcePtr<GPUTexture> positions;
        ResourcePtr<GPUTexture> albedo;
        ResourcePtr<GPUTexture> normals;
        ResourcePtr<GPUTexture> roughness_metalness_ao;
    };

    struct PointLightData {
        lib::math::Vector3f position;
        float attenuation;
        float range;
        lib::math::Vector3f color;
    };

    struct WorldData {
        lib::math::Matrixf view;
        lib::math::Matrixf projection;
        lib::math::Vector3f camera_position;
        uint32_t point_light_count;
        uint32_t direction_light_count;
        uint32_t spot_light_count;
    };

    struct ObjectData {
        lib::math::Matrixf model;
    };

    asset::AssetManager* _asset_manager;

    backend::Device _device;

    std::optional<lib::Receiver<asset::AssetEvent<asset::Mesh>>> _mesh_event_receiver;
    std::optional<lib::Receiver<asset::AssetEvent<asset::Technique>>> _technique_event_receiver;
    std::optional<lib::Receiver<asset::AssetEvent<asset::Texture>>> _texture_event_receiver;

    std::optional<UploadManager> _upload_manager;

    std::optional<ShaderCache> _shader_cache;
    std::optional<GeometryCache> _geometry_cache;
    std::optional<FrameGraph> _frame_graph;
    std::optional<PipelineCache> _pipeline_cache;
    std::optional<TextureCache> _texture_cache;

    std::vector<GBufferData> _gbuffers;
    std::vector<ResourcePtr<GPUTexture>> _depth_stencils;

    std::vector<PointLightData> _point_lights;

    std::vector<BufferPool<BufferPoolType::SBuffer, sizeof(ObjectData)>> _object_pools;
    std::vector<BufferPool<BufferPoolType::CBuffer, sizeof(WorldData)>> _world_pools;
    std::vector<BufferPool<BufferPoolType::CBuffer, 1024>> _material_pools;
    std::vector<BufferPool<BufferPoolType::SBuffer, sizeof(PointLightData)>> _point_light_pools;

    std::vector<uint8_t> _material_buffer;
    // std::vector<std::shared_ptr<GPUTexture>> _material_samplers;

    asset::AssetPtr<asset::Technique> _deffered_technique;
    asset::AssetPtr<asset::Technique> _lighting_technique;
    asset::AssetPtr<asset::Technique> _billboard_technique;
    asset::AssetPtr<asset::Mesh> _quad_mesh;

    std::vector<EditorInstance> _editor_instances;

    RenderQueue _deffered_queue;

    uint32_t _width;
    uint32_t _height;

    bool _editor_mode;

    void build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const frame_index, scene::CameraNode* camera);

    void apply_material_parameters(ShaderUniformBinder& binder, asset::Material& material, uint32_t const frame_index);
};

}