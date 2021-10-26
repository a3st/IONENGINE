// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"
#include "../platform/platform.h"

#include "frame_graph.h"

namespace ionengine::rendersystem {

class Renderer {

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

    struct {
        std::vector<std::unique_ptr<lgfx::Texture>> textures;
        std::vector<std::unique_ptr<lgfx::TextureView>> texture_views;
        std::vector<std::unique_ptr<lgfx::Fence>> fences;
        std::vector<uint64_t> fence_values;
    } frame_resources_;

    uint32_t frame_index_;

    lgfx::DescriptorPool frame_descriptor_pool_;
};

}