// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "renderer.h"

using namespace ionengine::rendersystem;

Renderer::Renderer(platform::Window* window) : window_(window) {

    device_ = lgfx::Device(0, window->GetNativeHandle(), 800, 600, 2, 1);
}

void Renderer::Frame() {

    struct BasicPass {

    };

    frame_graph_.AddTask<BasicPass>(FrameGraphTaskType::RenderPass,
        [&](FrameGraphTaskBuilder* builder, BasicPass& data) {

        },
        [=](FrameGraphTaskContext* context) {
            
        });

    frame_graph_.Execute();
}