#pragma once

#include "gapi/core.hpp"
#include "platform/window.hpp"
#include "core/ref_ptr.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <winrt/base.h>

namespace ie {

namespace gapi {

class Instance : public core::ref_counted_object {
    friend class Device;
    
public:

    Instance(bool const debug_);

    auto get_adapters() -> std::vector<AdapterDesc>;

private:
    bool debug;

    winrt::com_ptr<IDXGIFactory> factory;
};

class Device : public core::ref_counted_object {
public:

    Device(
        Instance& instance, 
        uint16_t const adapter_index, 
        core::ref_ptr<platform::Window> const window_ = nullptr
    );

    

private:

    core::ref_ptr<platform::Window> window;

    winrt::com_ptr<ID3D12Device> device;
    winrt::com_ptr<IDXGISwapChain> swapchain;

    winrt::com_ptr<ID3D12CommandQueue> direct_queue;
    winrt::com_ptr<ID3D12CommandQueue> copy_queue;
    winrt::com_ptr<ID3D12CommandQueue> compute_queue;
};

class Texture : public core::ref_counted_object {
public:

    Texture(
        Device& device, 
        TextureDimension const dimension,
        TextureFormat const format,
        uint32_t const width,
        uint32_t const height,
        uint32_t const depth,
        uint16_t const mip_levels,
        std::optional<SamplerDesc> const sampler_desc
    );

    auto get_width() const -> uint32_t;

    auto get_height() const -> uint32_t;

    auto get_depth() const -> uint32_t;

    auto get_mip_levels() const -> uint16_t;
    
private:

    
};

}

}