// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <ui/user_interface.h>
#include <RmlUi/Debugger.h>
#include <renderer/renderer.h>

using namespace ionengine;
using namespace ionengine::ui;

UserInterface::UserInterface(renderer::Renderer& renderer, platform::Window& window, lib::Logger& logger) : 
    _system_interface(logger),
    _render_interface(renderer.ui_renderer()) {

    Rml::SetRenderInterface(&_render_interface);
    Rml::SetSystemInterface(&_system_interface);
    Rml::Initialise();

    _context = Rml::CreateContext("main", Rml::Vector2i(window.client_width(), window.client_height()));

    Rml::LoadFontFace("engine/editor/fonts/Roboto-Regular.ttf");
    Rml::LoadFontFace("engine/fonts/LucidaConsole-Regular.ttf");

    Rml::Debugger::Initialise(_context);

    _debug_document = _context->LoadDocument("engine/ui/debug.html");
    _debug_document->Show();

    //_document_editor = _context->LoadDocument("engine/editor/ui/editor.rml");
    //_document_editor->Show();
    //_document->PullToFront();
}

void UserInterface::render_target_sel(asset::AssetPtr<asset::Texture> texture) {
    
    Rml::Element* element = _document_editor->GetElementById("editor_viewport_rt");
    
    Rml::ElementAttributes attributes;
    attributes.insert( std::pair<Rml::String, Rml::Variant> { "src", Rml::Variant(std::format("RT[{}x{}]{{{}}}", texture->as_const_ok().width, texture->as_const_ok().height, texture->as_const_ok().hash)) });
    attributes.insert( std::pair<Rml::String, Rml::Variant> { "width", Rml::Variant(std::format("{}px", texture->as_const_ok().width)) });
    attributes.insert( std::pair<Rml::String, Rml::Variant> { "height", Rml::Variant(std::format("{}px", texture->as_const_ok().height)) });
    element->SetAttributes(attributes);
}

void UserInterface::hot_reload() {
    _document_editor->Close();
    _document_editor = _context->LoadDocument("engine/editor/ui/editor.rml");
    _document_editor->ReloadStyleSheet();
    _document_editor->Show();
    _document_editor->PushToBack();
}

void UserInterface::show_debug(bool const show) {
    if(show) {
        _debug_document->Show();
    } else {
        _debug_document->Hide();
    }
}

void UserInterface::show_debugger(bool const show) {
    if(show) {
        Rml::Debugger::SetVisible(true);
    } else {
        Rml::Debugger::SetVisible(false);
    }
}

void UserInterface::element_text_fps(std::string_view const text) {
    Rml::Element* element = _debug_document->GetElementById("fps_count");
    element->SetInnerRML(std::string(text.data(), text.size()));
}

void UserInterface::element_text_frame_count(std::string_view const text) {
    Rml::Element* element = _debug_document->GetElementById("frame_count");
    element->SetInnerRML(std::string(text.data(), text.size()));
}

void UserInterface::element_text_frame_time(std::string_view const text) {
    Rml::Element* element = _debug_document->GetElementById("frame_time");
    element->SetInnerRML(std::string(text.data(), text.size()));
}

void UserInterface::element_text_gpu_name(std::string_view const text) {
    Rml::Element* element = _debug_document->GetElementById("gpu_name");
    element->SetInnerRML(std::string(text.data(), text.size()));
}

void UserInterface::element_text_gpu_memory_usage(std::string_view const text) {
    Rml::Element* element = _debug_document->GetElementById("gpu_memory_usage");
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
