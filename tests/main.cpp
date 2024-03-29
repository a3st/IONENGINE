
#include "core/exception.hpp"
#include "engine/engine.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

using namespace ionengine;

namespace project
{
    class MyEngine : public ionengine::Engine
    {
      public:
        MyEngine(core::ref_ptr<platform::Window> window) : Engine(window)
        {
        }

      protected:
        auto init() -> void override
        {
            model = asset_loader.load_model("models/cube.glb");
            shader_manager.load_shader("shaders/basic.bin");

            material = create_material("Basic");

            model->get_meshes()[0].material = material;

            //base_color = create_texture(800, 600, TextureFlags::RenderTarget);
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
} // namespace project

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        auto window = platform::Window::create("Example", 800, 600);
        auto engine = core::make_ref<project::MyEngine>(window);
        return engine->run();
    }
    catch (core::Exception e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}