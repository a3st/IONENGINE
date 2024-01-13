// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "renderer/render_graph.hpp"

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Backend;

class RenderPipeline : public core::ref_counted_object {
public:

    RenderPipeline() = default;

    virtual auto render(Backend& backend, platform::Window const& window) -> core::ref_ptr<RenderGraph> = 0;
};

}

}