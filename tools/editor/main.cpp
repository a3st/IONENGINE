// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "app.hpp"
#include "precompiled.h"
#include <core/exception.hpp>

using namespace ionengine;
namespace editor = ionengine::tools::editor;

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        editor::App app(argc, argv);
        return app.run();
    }
    catch (core::Exception e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

/*
    app.bind("newTestScene", [&](libwebview::EventArgs const& e) {
            scene = core::make_ref<editor::Scene>();

            auto outputNode = scene->createNode<editor::OutputNode>(600, 10);
            auto textureNode = scene->createNode<editor::Texture2DNode>(10, 10);
            textureNode->defineOption("textureName", "simple");
            auto samplerNode = scene->createNode<editor::Sampler2DNode>(400, 10);
            samplerNode->defineOption("UV", "input.uv");

            scene->createConnection(textureNode, 0, samplerNode, 1);
            scene->createConnection(samplerNode, 0, outputNode, 0);

            scene->dfs();

            app.result(e.index, true, scene->dump());
        });

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
*/