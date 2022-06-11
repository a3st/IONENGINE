// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <RmlUi/Core.h>
#include <ui/system_interface.h>
#include <ui/render_interface.h>
#include <asset/asset_manager.h>

namespace ionengine {

namespace platform {
class Window;
}

namespace renderer {
class Renderer;
}

}

namespace ionengine::ui {

class UserInterface {
public:

    UserInterface(renderer::Renderer& renderer, platform::Window& window);

    ~UserInterface();

    void update();

    Rml::Context& context();

    RenderInterface& render_interface();

    void element_text_fps(std::string_view const text);

    void element_text_frame_count(std::string_view const text);

    void element_text_frame_time(std::string_view const text);

    void element_text_gpu_name(std::string_view const text);

    void element_text_gpu_memory_usage(std::string_view const text);

    void hot_reload();

    void show_debug(bool const show);

    void show_debugger(bool const show);

    void render_target_sel(asset::AssetPtr<asset::Texture> texture);

private:

    SystemInterface _system_interface;
    RenderInterface _render_interface;

    Rml::Context* _context;

    Rml::ElementDocument* _debug_document;
    Rml::ElementDocument* _document_editor;
};

}