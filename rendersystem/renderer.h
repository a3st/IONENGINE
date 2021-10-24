// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"
#include "../platform/platform.h"

#include "frame_graph.h"

namespace ionengine::rendersystem {

class Renderer {

public:

    Renderer(platform::Window* window);

    void Frame();

private:

    void BeginFrame();
    void EndFrame();

    platform::Window* window_;

    lgfx::Device device_;
    FrameGraph frame_graph_;

    struct {
        std::vector<lgfx::Texture> textures;
        std::vector<lgfx::TextureView> texture_views;
        std::vector<lgfx::Fence> fences;
        std::vector<uint64_t> fence_values;
    } frame_resources_;

    uint32_t frame_index_;

    lgfx::DescriptorPool frame_descriptor_pool_;
};

}