// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/frame_graph.h>
#include <renderer/geometry_cache.h>
#include <renderer/texture_cache.h>
#include <renderer/rt_texture_cache.h>
#include <renderer/shader_cache.h>
#include <renderer/pipeline_cache.h>
#include <renderer/render_queue.h>
#include <renderer/buffer_pool.h>
#include <asset/asset_manager.h>
#include <lib/math/matrix.h>
#include <lib/channel.h>

namespace ionengine {

namespace scene {
class Scene;
class CameraNode;
};

}

namespace ionengine::renderer {

class DescriptorBinder;
struct NullData;

struct PointLightData {
    lib::math::Vector3f position;
    float range;
    lib::math::Vector3f color;
};

__declspec(align(backend::TEXTURE_ROW_PITCH_ALIGNMENT)) struct WorldData {
    lib::math::Matrixf view;
    lib::math::Matrixf proj;
    lib::math::Vector3f camera_position;
    lib::math::Matrixf inverse_view_proj;
};

__declspec(align(backend::TEXTURE_ROW_PITCH_ALIGNMENT)) struct LightData {
    uint32_t point_light_count;
};

uint32_t constexpr MATERIAL_DATA_SIZE = 1024;

struct ObjectData {
    lib::math::Matrixf model;
    lib::math::Matrixf inverse_model;
};

struct WorldEnvironmentData {
    asset::AssetPtr<asset::Material> skybox_material;
};

class MeshRenderer {
public:

    MeshRenderer(backend::Device& device, UploadManager& upload_manager, platform::Window& window, asset::AssetManager& asset_manager);

    ~MeshRenderer();

    MeshRenderer(MeshRenderer const&) = delete;

    MeshRenderer(MeshRenderer&&) = delete;

    MeshRenderer& operator=(MeshRenderer const&) = delete;

    MeshRenderer& operator=(MeshRenderer&&) = delete;

    void update(float const delta_time, std::queue<UploadBatch>& upload_batches);

    void resize(uint32_t const width, uint32_t const height);

    void render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, RTTextureCache& rt_texture_cache, NullData& null, FrameGraph& frame_graph, scene::Scene& scene, ResourcePtr<GPUTexture> swap_texture, uint32_t const frame_index);

    bool _is_ssr_enable{false};

private:

    struct GBufferData {
        ResourcePtr<GPUTexture> positions;
        ResourcePtr<GPUTexture> albedo;
        ResourcePtr<GPUTexture> normals;
        ResourcePtr<GPUTexture> roughness_metalness_ao;
    };

    backend::Device* _device;
    asset::AssetManager* _asset_manager;
    UploadManager* _upload_manager;

    std::optional<lib::Receiver<asset::AssetEvent<asset::Texture>>> _texture_receiver;
    std::optional<lib::Receiver<asset::AssetEvent<asset::Mesh>>> _mesh_receiver;

    TextureCache _texture_cache;
    GeometryCache _geometry_cache;

    std::vector<GBufferData> _gbuffers;
    std::vector<ResourcePtr<GPUTexture>> _depth_stencils;
    std::vector<ResourcePtr<GPUTexture>> _final_images;
    std::vector<ResourcePtr<GPUTexture>> _ssr_images;

    std::vector<PointLightData> _point_lights;
    RenderQueue _opaque_queue;
    RenderQueue _transculent_queue;
    scene::CameraNode* _render_camera;

    WorldEnvironmentData _world_environment;

    std::vector<BufferPool<BufferPoolType::SBuffer, sizeof(ObjectData)>> _object_pools;
    std::vector<BufferPool<BufferPoolType::CBuffer, sizeof(WorldData)>> _world_pools;
    std::vector<BufferPool<BufferPoolType::CBuffer, sizeof(LightData)>> _light_pools;
    std::vector<BufferPool<BufferPoolType::CBuffer, MATERIAL_DATA_SIZE>> _material_pools;
    std::vector<BufferPool<BufferPoolType::SBuffer, sizeof(PointLightData)>> _point_light_pools;

    ResourcePtr<GeometryBuffer> _quad;
    asset::AssetPtr<asset::Mesh> _cube;

    uint16_t _anisotropic;
    backend::Filter _filter;

    uint32_t _width;
    uint32_t _height;

    asset::AssetPtr<asset::Shader> _deffered_shader;
    asset::AssetPtr<asset::Shader> _fxaa_shader;
    asset::AssetPtr<asset::Shader> _ssr_shader;
    asset::AssetPtr<asset::Shader> _skybox_shader;

    void apply_material(DescriptorBinder& binder, GPUProgram const& program, asset::Material& material, uint32_t const frame_index);
};

}
