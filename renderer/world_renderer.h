// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/mesh_data.h>
#include <lib/thread_pool.h>
#include <lib/math/matrix.h>

namespace ionengine::renderer {

struct WorldBuffer {
    Matrixf model;
    Matrixf view;
    Matrixf projection;
};

class WorldRenderer {
public:

    WorldRenderer(Backend* const backend, ThreadPool* const thread_pool);

    WorldRenderer(WorldRenderer const&) = delete;

    WorldRenderer(WorldRenderer&&) = delete;

    WorldRenderer& operator=(WorldRenderer const&) = delete;

    WorldRenderer& operator=(WorldRenderer&&) = delete;

    void update();

    void draw_mesh(uint32_t const sort_index, MeshData const* const mesh_data, Matrixf const& model);

    void set_projection_view(Matrixf const& projection, Matrixf const& view);

private:

    Backend* _backend;
    ThreadPool* _thread_pool;

    std::vector<Handle<RenderPass>> rpasses;
    std::vector<Handle<Pipeline>> pipelines;
    std::vector<Handle<Shader>> shaders;

    Handle<DescriptorLayout> desc_layout;
    Handle<Buffer> buffer_vertex;
    Handle<Buffer> buffer_index;
    Handle<Buffer> constant_buffer;
    Handle<Sampler> sampler;
    Handle<Texture> texture_base;
    Handle<DescriptorSet> descriptor_set;

    uint32_t frame_index{0};
    uint32_t index_count;

    std::vector<MeshData const*> _meshes;
    std::vector<uint32_t> _draw_indices;
};

}