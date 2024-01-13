// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/render_pipeline.hpp"

namespace ionengine {

namespace renderer {

class MyRenderPipeline : public RenderPipeline {
public:

    MyRenderPipeline() = default;

    auto render(Backend& backend, platform::Window const& window) -> core::ref_ptr<RenderGraph> override;
};

}

}