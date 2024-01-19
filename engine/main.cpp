
#include "precompiled.h"
#include "engine.hpp"
#include "core/exception.hpp"

using namespace ionengine;

auto main(int32_t argc, char** argv) -> int32_t {

    try {
        Engine engine("IONENGINE project");
        engine.run();
    } catch(core::Exception e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}