// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <ui/user_interface.h>

using namespace ionengine;
using namespace ionengine::ui;

UserInterface::UserInterface(renderer::Renderer& renderer, platform::Window& window, lib::Logger& logger) : 
    _system_interface(logger) {

    Rml::SetRenderInterface(&_render_interface);
    Rml::SetSystemInterface(&_system_interface);
    Rml::Initialise();

    _context = Rml::CreateContext("main", Rml::Vector2i(window.client_width(), window.client_height()));
}

UserInterface::~UserInterface() {
    Rml::Shutdown();
}

void UserInterface::update() {

    _context->Update();
}
