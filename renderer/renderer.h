// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/frame_graph.h>
#include <scene/scene.h>
#include <lib/thread_pool.h>
#include <lib/math/matrix.h>

namespace ionengine::renderer {

class Renderer {
public:

    Renderer(uint32_t const adapter_index, backend::SwapchainDesc const& swapchain_desc, std::filesystem::path const& cache_path, lib::ThreadPool& thread_pool);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer const&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    void render(scene::Scene& scene);

    void resize(uint32_t const width, uint32_t const height);

private:

    lib::ThreadPool* _thread_pool;

    backend::Backend _backend;
    //ShaderCache _shader_cache;

    std::vector<backend::Encoder> _graphics_encoders;
    std::vector<backend::FenceResultInfo> _graphics_fence_results;
    std::vector<backend::Encoder> _copy_encoders;
    std::vector<backend::FenceResultInfo> _copy_fence_results;

    uint32_t _frame_count{0};
    uint32_t _frame_index{0};

    uint16_t _sample_count{0};

    FrameGraph _frame_graph;

    enum class LayoutType {
        ZPass,
        GBuffer,
        PostProcess
    };

    std::unordered_map<LayoutType, Handle<backend::DescriptorLayout>> _layouts;

    void initialize_resources_per_frame();

    void initialize_shaders();

    void initialize_layouts();

    void build_frame_graph(uint32_t const width, uint32_t const height, uint16_t const sample_count, uint32_t const buffer_count);

    // TEST
    std::vector<Handle<backend::Pipeline>> test_pipelines;
    Handle<backend::Buffer> test_vertex_triangle;
    Handle<backend::Buffer> test_index_triangle;
    Handle<backend::DescriptorSet> test_descriptor_set;
};

}