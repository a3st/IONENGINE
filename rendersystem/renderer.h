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

    platform::Window* window_;

    lgfx::Device device_;

    FrameGraph frame_graph_;

    struct FrameResource {
        lgfx::Texture texture;
        lgfx::TextureView view;
    };

    std::vector<FrameResource> frame_resources_;
    lgfx::DescriptorPool frame_descriptor_pool_;
};

}