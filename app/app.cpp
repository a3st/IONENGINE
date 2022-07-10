// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/include/platform/window.hpp>

using namespace ionengine;

int main(int* argc, char** agrv) {

    auto result = platform::Window::create(800, 600, "Test");
    
    if(result.has_error()) {
        std::cerr << result.error() << std::endl;
        return 0;
    }

    auto window = std::move(result.ok());

    platform::poll_events(*window, []() { });

    return 0;
}