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

    struct Frame {
        Handle<Buffer> vertex_buffer;
        Handle<Buffer> index_buffer;
    };

    std::vector<Handle<RenderPass>> _render_passes;
    std::vector<Handle<Pipeline>> _pipelines;
    std::vector<Handle<Shader>> _shaders;
    std::vector<Handle<DescriptorSet>> _descriptor_sets;
    std::vector<Handle<Buffer>> _constant_buffers;

    Handle<DescriptorLayout> _pbr_layout;

    Handle<Sampler> _sampler;
    Handle<Texture> _texture;

    std::vector<Frame> _frames;
    uint32_t frame_index{0};

    std::vector<MeshData const*> _meshes;
    std::vector<uint32_t> _draw_indices;

    WorldBuffer _world_buffer;
};

}