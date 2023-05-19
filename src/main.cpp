// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/window.hpp"
#include "gapi/gapi.hpp"

#include "compositor/microshader.hpp"

using namespace ie;

auto main(int32_t argc, char** argv) -> int32_t {
    ::SetConsoleOutputCP(CP_UTF8);

    try {
        auto window = core::make_ref<platform::Window>("App", 800, 600);
        auto instance = core::make_ref<gapi::Instance>(true);

        auto adapters = instance->get_adapters();

        for(auto const& adapter : adapters) {
            std::cout << std::format("{} {} {} {}", adapter.index, adapter.name, adapter.memory_size, adapter.is_uma) << std::endl;
        }

        auto device = core::make_ref<gapi::Device>(&instance, adapters[0].index, window);
        
        compositor::MicroShader pbr("shaders/pbr.hlsl");

        std::cout << std::format("{} {}", pbr.get_shader_name(), pbr.get_entry_func()) << std::endl;

        window->run();

    } catch(std::exception e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}