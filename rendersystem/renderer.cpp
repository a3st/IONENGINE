// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "renderer.h"

using namespace ionengine::rendersystem;

Renderer::Renderer(platform::Window* window) : 
    window_(window),
    device_(0, window->GetNativeHandle(), 800, 600, 2, 1),
    frame_graph_(&device_),
    frame_descriptor_pool_(&device_, 2, lgfx::DescriptorType::kRenderTarget, lgfx::DescriptorFlags::kNone) {

    frame_resources_.textures.resize(2);
    frame_resources_.texture_views.resize(2);
    frame_resources_.fences.resize(2);
    frame_resources_.fence_values.resize(2);

    lgfx::TextureViewDesc view_desc{};
    view_desc.dimension = lgfx::Dimension::kTexture2D;
    for(uint32_t i = 0; i < 2; ++i) {
        frame_resources_.textures[i] = std::make_unique<lgfx::Texture>(&device_, i);
        frame_resources_.texture_views[i] = std::make_unique<lgfx::TextureView>(&device_, &frame_descriptor_pool_, frame_resources_.textures[i].get(), view_desc);
        frame_resources_.fences[i] = std::make_unique<lgfx::Fence>(&device_, 0);
    }

    // Main DescriptorLayout
    {
        std::vector<lgfx::DescriptorLayoutBinding> bindings = {
            { lgfx::DescriptorType::kConstantBuffer, 0, 0, 3, lgfx::ShaderModuleType::kVertex }
        };

        descriptor_layouts_.emplace_back(std::make_unique<lgfx::DescriptorLayout>(&device_, bindings));
    }

    auto read_shader_file = [](const std::filesystem::path& shader_path) -> std::vector<std::byte> {

        std::vector<std::byte> shader_blob;

        std::ifstream ifs(shader_path, std::ios::binary);
        if(!ifs.is_open()) {
            throw std::runtime_error("Shader blob error loading");
        }

        ifs.seekg(0, std::ios::end);
        size_t size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        shader_blob.resize(size);
        ifs.read(reinterpret_cast<char*>(shader_blob.data()), shader_blob.size());
        return shader_blob;
    };

    std::vector<std::byte> vertex = read_shader_file("shaders/basic_vert.bin");
    std::vector<std::byte> pixel = read_shader_file("shaders/basic_frag.bin");

    std::vector<lgfx::InputLayoutDesc> inputs = {
        { "POSITION", 0, lgfx::Format::kRGB32float, 0, 0 }
    };

    std::vector<lgfx::ShaderModuleDesc> shaders = {
        { lgfx::ShaderModuleType::kVertex, vertex },
        { lgfx::ShaderModuleType::kPixel, pixel }
    };

    lgfx::RenderPassDesc pass_desc{};
    
    render_pass_ = std::make_unique<lgfx::RenderPass>(&device_, pass_desc);

    lgfx::PipelineDesc pipeline_desc{};
    pipeline_desc.type = lgfx::PipelineType::kGraphics;
    pipeline_desc.layout = descriptor_layouts_[0].get();
    pipeline_desc.inputs = inputs;
    pipeline_desc.shaders = shaders;
    pipeline_desc.render_pass = render_pass_.get();

    pipeline_ = std::make_unique<lgfx::Pipeline>(&device_, pipeline_desc);

    descriptor_set_ = std::make_unique<lgfx::DescriptorSet>(&device_);
}

void Renderer::BeginFrame() {

    frame_index_ = device_.GetSwapchainBufferIndex();
}

void Renderer::Frame() {

    BeginFrame();

    FrameGraphTask* basic_task = frame_graph_.AddTask(FrameGraphTaskType::kRenderPass,
        [&](FrameGraphBuilder* builder) {

            FrameGraphResource* swapchain = builder->Create(FrameGraphResourceDesc {
                FrameGraphResourceType::kAttachment,
                0, 0, 0, 0, 
                lgfx::Format::kUnknown, 
                lgfx::TextureFlags::kRenderTarget,
                frame_resources_.textures[frame_index_].get(), frame_resources_.texture_views[frame_index_].get(), true });

            FrameGraphResource* dummy = builder->Create(FrameGraphResourceDesc {
                FrameGraphResourceType::kAttachment,
                800, 600, 1, 1,
                lgfx::Format::kRGBA8unorm,
                lgfx::TextureFlags::kRenderTarget });

            builder->Write(swapchain, FrameGraphResourceOp::kClear, Color { 0.4f, 0.5f, 0.3f, 1.0f });
            builder->Write(dummy, FrameGraphResourceOp::kClear, Color { 0.4f, 0.5f, 0.9f, 1.0f });
        },
        [=](FrameGraphContext* context) {
            
            context->GetBuffer()->BindPipeline(pipeline_.get());
            context->GetBuffer()->BindDescriptorSet(descriptor_set_.get());
            
        });

    frame_graph_.Execute();

    EndFrame();
}

void Renderer::EndFrame() {

    const uint64_t fence_value = frame_resources_.fence_values[frame_index_];
    device_.Signal(lgfx::CommandBufferType::kGraphics, frame_resources_.fences[frame_index_].get(), fence_value);
    ++frame_resources_.fence_values[frame_index_];
    frame_resources_.fences[frame_index_]->Wait(fence_value);
}

void Renderer::Resize(const uint32_t width, const uint32_t height) {

}