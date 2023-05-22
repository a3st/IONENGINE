// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/window.hpp"
#include "gapi/gapi.hpp"

#include "compositor/shader_function.hpp"
#include "compositor/shader_resource.hpp"

using namespace ie;

auto main(int32_t argc, char** argv) -> int32_t {
    ::SetConsoleOutputCP(CP_UTF8);

    try {
        auto window = core::make_ref<platform::Window>("Engine Test", 800, 600);
        auto instance = core::make_ref<gapi::Instance>(true);

        auto adapters = instance->get_adapters();

        for(auto const& adapter : adapters) {
            std::cout << std::format("{} {} {} {}", adapter.index, adapter.name, adapter.memory_size, adapter.is_uma) << std::endl;
        }

        auto device = core::make_ref<gapi::Device>(&instance, adapters[0].index, window);
        
        compositor::ShaderFunction pbr("shaders/pbr.hlsl");

        std::cout << std::format("{} {} out {} in {}", pbr.get_shader_name(), pbr.get_entry_func(), pbr.get_out_params().size(), pbr.get_in_params().size()) << std::endl;

        compositor::ShaderFunction split_rgba("shaders/split_rgba.hlsl");

        std::cout << std::format("{} {} out {} in {}", 
            split_rgba.get_shader_name(), 
            split_rgba.get_entry_func(), 
            split_rgba.get_out_params().size(), 
            split_rgba.get_in_params().size()) << std::endl;

        compositor::ShaderResource sampler2D(compositor::ShaderResourceType::Sampler2D, compositor::ShaderParameterType::Float3);

        window->run();

    } catch(std::exception e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}