
#include "core/exception.hpp"
#include "engine/engine.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

using namespace ionengine;

namespace ionengine
{
    class MyEngine : public Engine
    {
      public:
        MyEngine(core::ref_ptr<platform::Window> window)
            : Engine(window)
        {
        }

      protected:
        auto init() -> void override
        {
        }

        auto update(float const dt) -> void override
        {
        }

        auto render() -> void override
        {
        }
    };
} // namespace ionengine

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        auto window = platform::Window::create("Example", 800, 600);

        auto engine =
            core::make_ref<ionengine::MyEngine>(window);
        
        return engine->run();
    }
    catch (core::Exception e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}