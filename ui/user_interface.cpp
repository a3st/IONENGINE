// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <ui/user_interface.h>
#include <RmlUi/Debugger.h>

using namespace ionengine;
using namespace ionengine::ui;

UserInterface::UserInterface(platform::Window& window, lib::Logger& logger) : 
    _system_interface(logger) {

    Rml::SetRenderInterface(&_render_interface);
    Rml::SetSystemInterface(&_system_interface);
    Rml::Initialise();

    _context = Rml::CreateContext("main", Rml::Vector2i(window.client_width(), window.client_height()));

    Rml::LoadFontFace("content/ui/LatoLatin-Regular.ttf");
    Rml::LoadFontFace("content/ui/LucidaConsole-Regular.ttf");

    _document = _context->LoadDocument("content/ui/demo.rml");

    _document->Show();

    Rml::Debugger::Initialise(_context);
}

void UserInterface::element_text(std::string_view const text) {
    Rml::Element* element = _document->GetElementById("fps_count");
    element->SetInnerRML(std::string(text.data(), text.size()));
}

void UserInterface::element_text_2(std::string_view const text) {
    Rml::Element* element = _document->GetElementById("frame_count");
    element->SetInnerRML(std::string(text.data(), text.size()));
}

void UserInterface::element_text_3(std::string_view const text) {
    Rml::Element* element = _document->GetElementById("frame_time");
    element->SetInnerRML(std::string(text.data(), text.size()));
}

UserInterface::~UserInterface() {
    Rml::Shutdown();
}

void UserInterface::update() {
    _context->Update();
}

Rml::Context& UserInterface::context() {
    return *_context;
}

RenderInterface& UserInterface::render_interface() {
    return _render_interface;
}
