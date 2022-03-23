// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/frame_graph.h>
#include <renderer/material_manager.h>
#include <renderer/texture_manager.h>
#include <renderer/shader.h>
#include <renderer/data.h>
#include <lib/thread_pool.h>
#include <lib/math/matrix.h>

namespace ionengine::renderer {

enum class TextureQuality : uint32_t {
    Low,
    Medium,
    High,
    Ultra
};

enum class MeshQuality : uint32_t {
    Low,
    Medium,
    High
};

enum class ShadowQuality : uint32_t {
    Low,
    High
};

enum class MaterialQuality : uint32_t {
    Low,
    High
};

enum class AnisotropicQuality : uint32_t {
    Linear
};

struct RendererQuality {

};

struct WorldBuffer {
    Matrixf model;
    Matrixf view;
    Matrixf projection;
};

class WorldRenderer {
public:

    WorldRenderer(Backend& backend, ThreadPool& thread_pool, ShaderPackageData const& shader_package_data);

    WorldRenderer(WorldRenderer const&) = delete;

    WorldRenderer(WorldRenderer&&) = delete;

    WorldRenderer& operator=(WorldRenderer const&) = delete;

    WorldRenderer& operator=(WorldRenderer&&) = delete;

    void update();

    void resize(uint32_t const width, uint32_t const height);

    void draw_mesh(uint32_t const sort_index, MeshSurfaceData const* const mesh_data, Matrixf const& model);

    void set_projection_view(Matrixf const& projection, Matrixf const& view);

private:

    Backend* _backend;
    ThreadPool* _thread_pool;

    FrameGraph _frame_graph;
    MaterialManager _material_manager;

    ShaderCache _shader_cache;

    std::vector<Handle<DescriptorSet>> _descriptor_sets;
    std::vector<Handle<Buffer>> _constant_buffers;

    std::vector<Encoder> _graphics_encoders;
    std::vector<FenceResultInfo> fence_results;

    Handle<Pipeline> _pipeline_test;

    Handle<DescriptorLayout> _pbr_layout;

    uint32_t _frame_index{0};

    void initialize_shaders(ShaderPackageData const& shader_package_data);

    void initialize_descriptor_layouts();

    void build_frame_graph(uint32_t const width, uint32_t const height, uint16_t const sample_count, uint32_t const buffer_count);
};

}