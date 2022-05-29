// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <RmlUi/Core.h>
#include <ui/render_interface.h>
#include <ui/system_interface.h>
#include <renderer/renderer.h>

namespace ionengine::platform {
class Window;
}

namespace ionengine::ui {

class UserInterface {
public:

    UserInterface(renderer::Renderer& renderer, platform::Window& window, lib::Logger& logger);

    ~UserInterface();

    void update();

private:

    Rml::Context* _context;

    SystemInterface _system_interface;
    RenderInterface _render_interface;
};

}