// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>
#include "backend.hpp"
#include "platform/window.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

auto Backend::get_win32_surface(wgpu::Instance instance, platform::Window& window) -> wgpu::Surface {

    auto get_hwnd_descriptor = [](platform::Window& window) -> std::shared_ptr<wgpu::ChainedStruct> {
        
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

Backend::Backend(platform::Window& window) {

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

    {
        auto descriptor = wgpu::SwapChainDescriptor {};
        descriptor.usage = wgpu::TextureUsage::RenderAttachment;
        descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
        descriptor.width = window.get_width();
        descriptor.height = window.get_height();
        descriptor.presentMode = wgpu::PresentMode::Fifo;
        
        swapchain = device.createSwapChain(surface, descriptor);
    }

    error_callback = device.setUncapturedErrorCallback([](wgpu::ErrorType type, char const * message) -> void {
        std::cout << type << " " << message << std::endl;
    });
}

auto Backend::update() -> void {

    device.tick();
}