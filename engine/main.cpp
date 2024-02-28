
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"
#include "engine.hpp"

using namespace ionengine;

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        auto window = platform::Window::create("Example", 800, 600);

        Engine engine("shaders", window);

        auto future = engine.load_model("models/vehicle-1mat.glb");
        future.wait();

        engine.run();
        return EXIT_SUCCESS;
    }
    catch (core::Exception e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}