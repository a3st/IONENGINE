// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/frame_graph.h>
#include <renderer/shader_graph.h>
#include <renderer/data.h>
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

    WorldRenderer(Backend& backend, ThreadPool& thread_pool, std::span<ShaderData const> const shaders);

    WorldRenderer(WorldRenderer const&) = delete;

    WorldRenderer(WorldRenderer&&) = delete;

    WorldRenderer& operator=(WorldRenderer const&) = delete;

    WorldRenderer& operator=(WorldRenderer&&) = delete;

    void update();

    void resize(uint32_t const width, uint32_t const height);

    void draw_mesh(uint32_t const sort_index, MeshData const* const mesh_data, Matrixf const& model);

    void set_projection_view(Matrixf const& projection, Matrixf const& view);

private:

    Backend* _backend;
    ThreadPool* _thread_pool;

    FrameGraph _frame_graph;

    std::vector<Handle<RenderPass>> _render_passes;
    std::vector<Handle<Pipeline>> _pipelines;
    std::vector<Handle<DescriptorSet>> _descriptor_sets;
    std::vector<Handle<Buffer>> _constant_buffers;

    std::vector<Encoder> _graphics_encoders;
    std::vector<FenceResultInfo> fence_results;

    Handle<DescriptorLayout> _pbr_layout;

    uint32_t _frame_index{0};

    std::vector<MeshData const*> _meshes;
    std::vector<uint32_t> _draw_indices;

    WorldBuffer _world_buffer;
    WorldBuffer _prev_world_buffer;

    std::unordered_map<uint32_t, ShaderTemplate> _shader_templates;

    void initialize_shaders(std::span<ShaderData const> const shaders);
    void initialize_descriptor_layouts();
    void build_frame_graph(uint32_t const width, uint32_t const height, uint16_t const sample_count, uint32_t const buffer_count);
};

}