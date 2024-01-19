// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "platform/window.hpp"
#include "platform/window_loop.hpp"
#include "renderer/renderer.hpp"

namespace ionengine {

class Engine {
public:

    Engine(
        std::string_view const title
    );

    auto run() -> void;

private:

    core::ref_ptr<platform::WindowLoop> window_loop{nullptr};
    core::ref_ptr<platform::Window> window{nullptr};
    core::ref_ptr<renderer::Renderer> renderer{nullptr};
};

}