// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/frame_graph.h>
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

class Renderer {
public:

    Renderer(Backend& backend, ThreadPool& thread_pool, ShaderPackageData const& shader_package_data);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void update();

    void resize(uint32_t const width, uint32_t const height);

private:

    Backend* _backend;
    ThreadPool* _thread_pool;

    ShaderCache _shader_cache;

    std::vector<Encoder> _graphics_encoders;
    std::vector<FenceResultInfo> _graphics_fence_results;
    std::vector<Encoder> _copy_encoders;
    std::vector<FenceResultInfo> _copy_fence_results;

    uint32_t _frame_count{0};
    uint32_t _frame_index{0};

    uint16_t _sample_count{0};

    FrameGraph _frame_graph;

    enum class LayoutType {
        ZPass,
        GBuffer,
        PostProcess
    };

    std::unordered_map<LayoutType, Handle<DescriptorLayout>> _layouts;

    void initialize_resources_per_frame();

    void initialize_shaders(ShaderPackageData const& shader_package_data);

    void initialize_layouts();

    void build_frame_graph(uint32_t const width, uint32_t const height, uint16_t const sample_count, uint32_t const buffer_count);

    // TEST
    std::vector<Handle<Pipeline>> test_pipelines;
    Handle<Buffer> test_vertex_triangle;
    Handle<Buffer> test_index_triangle;
    Handle<DescriptorSet> test_descriptor_set;
};

}