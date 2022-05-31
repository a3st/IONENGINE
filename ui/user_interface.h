// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <RmlUi/Core.h>
#include <ui/system_interface.h>
#include <ui/render_interface.h>

namespace ionengine::platform {
class Window;
}

namespace ionengine::ui {

class UserInterface {
public:

    UserInterface(platform::Window& window, lib::Logger& logger);

    ~UserInterface();

    void update();

    Rml::Context& context();

    RenderInterface& render_interface();

    void element_text(std::string_view const text);

    void element_text_2(std::string_view const text);

    void element_text_3(std::string_view const text);

private:

    Rml::Context* _context;
    Rml::ElementDocument* _document;

    SystemInterface _system_interface;
    RenderInterface _render_interface;
};

}