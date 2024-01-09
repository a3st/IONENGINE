// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <webgpu/webgpu.hpp>

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Backend {
public:

    Backend(platform::Window& window);

    auto get_device() const -> wgpu::Device {

        return device;
    }

    auto get_queue() const -> wgpu::Queue {

        return queue;
    }

    auto get_swapchain() const -> wgpu::SwapChain {

        return swapchain;
    }

    auto update() -> void;

private:

    wgpu::Instance instance{nullptr};
    wgpu::Surface surface{nullptr};
    wgpu::Adapter adapter{nullptr};
    wgpu::Device device{nullptr};
    wgpu::Queue queue{nullptr};
    wgpu::SwapChain swapchain{nullptr};

    std::unique_ptr<wgpu::ErrorCallback> error_callback;

    auto get_win32_surface(wgpu::Instance instance, platform::Window& window) -> wgpu::Surface;

    std::binary_semaphore semaphore{0};
};

}

}