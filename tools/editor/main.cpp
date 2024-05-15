// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include <base64pp/base64pp.h>
#include <webview.hpp>
#include <core/exception.hpp>

using namespace ionengine;
/*
using namespace ionengine;

namespace project
{
    class MyEngine : public ionengine::Engine
    {
      public:
        MyEngine(core::ref_ptr<platform::Window> window, libwebview::App& app) : Engine(window)
        {
            app.bind("requestRenderImage", [&](libwebview::EventArgs const& e) {
                auto buffer = base_color->dump();
                std::string const b64image = base64pp::encode(buffer);
                std::string data = std::format("{{\"image\":\"data:image/png;base64,{}\"}}", b64image);
                app.result(e.index, true, data);
            });
        }

      protected:
        auto init() -> void override
        {
            model = asset_loader.load_model("models/cube.glb");
            shader_manager.load_shader("shaders/basic.bin");

            material = create_material("Basic");

            model->get_meshes()[0].material = material;

            base_color = create_texture(752, 286, TextureFlags::RenderTarget);
        }

        auto update(float const dt) -> void override
        {
        }

        auto render() -> void override
        {
            renderer.begin_draw(std::vector<core::ref_ptr<Texture>>{base_color}, nullptr, {}, 0.0f, 0x0);
            renderer.draw_mesh(model->get_meshes()[0]);
            renderer.end_draw();
        }

      private:
        core::ref_ptr<ionengine::Model> model;
        core::ref_ptr<ionengine::Texture> base_color;
        core::ref_ptr<ionengine::Material> material;
    };
} // namespace project*/

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        libwebview::App app("ionengine", "Editor", 800, 600, true, true);
        //auto engine = core::make_ref<project::MyEngine>(nullptr, app);
        //engine->run();
        app.idle([&]() { /*engine->tick();*/ });
        if (!app.run("resources/index.html"))
        {
            throw core::Exception("An error occurred while creating the interface");
        }
        return EXIT_SUCCESS;
    }
    catch (core::Exception e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}