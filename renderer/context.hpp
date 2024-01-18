// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <webgpu/webgpu.hpp>

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Context {
public:

    Context(platform::Window const& window);

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

    auto recreate_swapchain(uint32_t const width, uint32_t const height) -> void;

private:

    wgpu::Instance instance{nullptr};
    wgpu::Surface surface{nullptr};
    wgpu::Adapter adapter{nullptr};
    wgpu::Device device{nullptr};
    wgpu::Queue queue{nullptr};
    wgpu::SwapChain swapchain{nullptr};

    std::unique_ptr<wgpu::ErrorCallback> error_callback;
    std::unique_ptr<wgpu::QueueWorkDoneCallback> work_done_callback;

    std::binary_semaphore semaphore{0};

    auto get_win32_surface(wgpu::Instance instance, platform::Window const& window) -> wgpu::Surface;
};

}

}