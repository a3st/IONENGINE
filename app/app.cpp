// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>

#include <platform/include/platform/window.hpp>
#include <renderer/device.hpp>
#include <renderer/texture.hpp>
#include <renderer/command_list.hpp>
#include <renderer/render_pass.hpp>

#include <core/include/core/array.hpp>
#include <core/include/core/vector.hpp>

using namespace ionengine;

int main(int* argc, char** agrv) {

    auto array = core::make_array<float, 4>();
    
    array[0] = 1.0f;

    std::cout << array[0] << std::endl;

    auto vector = core::make_vector<float>();

    vector.push(1.0f);
    vector.push(2.0f);
    vector.push(3.0f);
    vector.push(4.0f);

    

    for(size_t i = 0; i < vector.size(); ++i) {
        std::cout << vector[i] << std::endl;
    }


    std::unique_ptr<platform::Window> window;
    {
        auto result = platform::Window::create(800, 600, "Test");

        if (!result.has_value()) {
            std::cerr << result.error() << std::endl;
            return 0;
        }

        window = std::move(result.ok());
    }

    std::unique_ptr<renderer::Device> device;
    {
        auto result = renderer::Device::create(*window, 1);

        if (!result.has_value()) {
            std::cerr << result.error() << std::endl;
            return 0;
        }

        device = std::move(result.ok());
    }

    std::unique_ptr<renderer::Texture> texture;
    {
        auto result = renderer::Texture::create(
            *device, renderer::TextureDimension::_2D, 800, 600, 1, 1,
            renderer::TextureFormat::RGBA8_UNORM,
            renderer::TextureUsageFlags::ShaderResource);

        if (!result.has_value()) {
            std::cerr << result.error() << std::endl;
            return 0;
        }

        texture = std::move(result.ok());
    }

    std::array<std::unique_ptr<renderer::Texture>, renderer::BACK_BUFFER_COUNT> back_buffers;
    {
        for(size_t i = 0; i < back_buffers.size(); ++i) {
            auto result = renderer::Texture::from_swapchain(*device, i);

            if(!result.has_value()) {
                std::cerr << result.error() << std::endl;
                return 0;
            }

            back_buffers[i] = std::move(result.ok());
        }
    }

    /*std::unique_ptr<renderer::CommandList> command_list;
    {
        auto result = renderer::CommandList::create(*device, renderer::CommandListType::Direct, false);

        if(!result.has_value()) {
            std::cerr << result.error() << std::endl;
            return 0;
        }

        command_list = std::move(result.ok());
    }

    std::unique_ptr<renderer::RenderPass> render_pass;
    {
        std::vector<renderer::RenderPassColorDesc> color_descs(2);
        std::vector<renderer::Texture*> colors = { back_buffers[0].get(), back_buffers[1].get() };

        auto result = renderer::RenderPass::create(*device, colors, color_descs, nullptr, std::nullopt);
    }*/

    platform::poll_events(*window, [](platform::WindowEvent const& event) {
        switch (event.event_type) {
            case platform::WindowEventType::Closed: {
                std::cout << "Window close" << std::endl;
            } break;
            case platform::WindowEventType::Sized: {
                std::cout << std::format("Window resized to {}, {}",
                                         event.data.window.width,
                                         event.data.window.height)
                          << std::endl;
            } break;

            case platform::WindowEventType::GamepadButton: {
                std::cout << std::format("Gamepad key {0}, state {1}",
                                         event.data.input.key,
                                         (int32_t)event.data.input.state)
                          << std::endl;
            } break;

            case platform::WindowEventType::GamepadLeftAxis: {
                std::cout << std::format("Gamepad left axis {0}, {1}",
                                         event.data.input.x, event.data.input.y)
                          << std::endl;
            } break;
        }
    });

    return 0;
}