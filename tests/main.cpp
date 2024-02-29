
#include "core/exception.hpp"
#include "engine/engine.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

using namespace ionengine;

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        auto window = platform::Window::create("Example", 800, 600);
        auto engine = core::make_ref<Engine>(std::vector<std::filesystem::path>{"shaders/basic.bin"}, window);

        engine->on_render = [&](RenderContext& ctx) {
            
        };

        engine->run();
        return EXIT_SUCCESS;
    }
    catch (core::Exception e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}