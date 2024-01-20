// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>
#include "context.hpp"
#include "platform/window.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

auto Context::get_win32_surface(wgpu::Instance instance, platform::Window const& window) -> wgpu::Surface {

    auto get_hwnd_descriptor = [](platform::Window const& window) -> std::shared_ptr<wgpu::ChainedStruct> {
        
        auto hwnd_descriptor = std::make_shared<wgpu::SurfaceDescriptorFromWindowsHWND>();
        hwnd_descriptor->hwnd = window.get_native_handle();
        return std::reinterpret_pointer_cast<wgpu::ChainedStruct>(hwnd_descriptor);
    };

    auto hwnd_descriptor = get_hwnd_descriptor(window); 
    hwnd_descriptor->sType = wgpu::SType::SurfaceDescriptorFromWindowsHWND;

    auto descriptor = wgpu::SurfaceDescriptor {};
    descriptor.nextInChain = hwnd_descriptor.get();
    
    return instance.createSurface(descriptor);
}

Context::Context(platform::Window const& window) {

    {
        auto descriptor = wgpu::InstanceDescriptor {};
        instance = wgpu::createInstance(descriptor);

        if(!instance) {
            throw core::Exception("Error during creation WebGPU instance");
        }
    }

    surface = get_win32_surface(instance, window);

    {
        auto options = wgpu::RequestAdapterOptions {};
        options.compatibleSurface = surface;
        options.powerPreference = wgpu::PowerPreference::HighPerformance;

        instance.requestAdapter(options, [&](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, char const* message) -> void {
            if(status == wgpu::RequestAdapterStatus::Success) {
                this->adapter = adapter;
                semaphore.release();
            }
        });

        semaphore.acquire();
    }

    {
        auto descriptor = wgpu::DeviceDescriptor {};
        
        adapter.requestDevice(descriptor, [&](wgpu::RequestDeviceStatus status, wgpu::Device device, char const * message) -> void {
            if(status == wgpu::RequestDeviceStatus::Success) {
                this->device = device;
                semaphore.release();
            }
        });

        semaphore.acquire();
    }

    queue = device.getQueue();

    // recreate_swapchain(window.get_width(), window.get_height());

    error_callback = device.setUncapturedErrorCallback([](wgpu::ErrorType type, char const * message) -> void {
        std::cout << type << " " << message << std::endl;
    });

    work_done_callback = queue.onSubmittedWorkDone(0, [](wgpu::QueueWorkDoneStatus status) -> void {
        std::cout << "Submitted work done with " << status << std::endl;
    });
}

auto Context::update() -> void {

    device.tick();
}

auto Context::recreate_swapchain(uint32_t const width, uint32_t const height) -> void {

    {
        auto descriptor = wgpu::SwapChainDescriptor {};
        descriptor.usage = wgpu::TextureUsage::RenderAttachment;
        descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
        descriptor.width = width;
        descriptor.height = height;
        descriptor.presentMode = wgpu::PresentMode::Fifo;
        
        swapchain = device.createSwapChain(surface, descriptor);
    }
}

