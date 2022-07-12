// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>

#include <platform/include/platform/window.hpp>
#include <renderer/device.hpp>
#include <renderer/texture.hpp>

using namespace ionengine;

int main(int* argc, char** agrv) {
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