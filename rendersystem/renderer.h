// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"
#include "../platform/platform.h"

#include "frame_graph.h"

#include "cache/pipeline.h"
#include "cache/buffer.h"
#include "cache/buffer_view.h"

namespace ionengine::rendersystem {

class Renderer {

const size_t kRendererStageBufferSize = 67108864;

public:

    Renderer(platform::Window* window);
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;

    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void Frame();
    void Resize(const uint32_t width, const uint32_t height);

private:

    void BeginFrame();
    void EndFrame();

    platform::Window* window_;

    lgfx::Device device_;
    FrameGraph frame_graph_;

    lgfx::DescriptorPool frame_descriptor_pool_;

    lgfx::DescriptorPool test_descriptor_pool;
    lgfx::MemoryPool test_memory_pool;

    PipelineCache pipeline_cache_;
    BufferCache buffer_cache_;
    BufferViewCache buffer_view_cache_;

    struct {
        std::vector<std::unique_ptr<lgfx::Texture>> textures;
        std::vector<std::unique_ptr<lgfx::TextureView>> texture_views;
        std::vector<std::unique_ptr<lgfx::Fence>> fences;
        std::vector<uint64_t> fence_values;
    } frame_resources_;

    std::vector<std::unique_ptr<lgfx::DescriptorLayout>> descriptor_layouts_;

    std::unique_ptr<lgfx::DescriptorSet> descriptor_set_;

    std::unique_ptr<lgfx::Buffer> stage_buffer_;

    std::vector<lgfx::InputLayoutDesc> inputs_;
    std::vector<std::unique_ptr<lgfx::Shader>> shaders_;

    std::vector<lgfx::Shader*> shaders_c_;

    uint32_t frame_index_;

    std::vector<float> triangle_data_;
};

}