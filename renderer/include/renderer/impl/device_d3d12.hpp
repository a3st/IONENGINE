// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/device.hpp>
#include <renderer/impl/d3d12.hpp>

namespace ionengine::renderer {

///
/// @private
///
class Device_D3D12 final : public Device {
 public:
    static core::Expected<std::unique_ptr<Device>, std::string> create(
        platform::Window& window, uint16_t const sample_count) noexcept;

 private:
#ifdef _DEBUG
    WRL::ComPtr<ID3D12Debug> _debug;
#endif
    WRL::ComPtr<IDXGIFactory6> _factory;
    WRL::ComPtr<IDXGIAdapter1> _adapter;
    WRL::ComPtr<ID3D12Device4> _device;
    WRL::ComPtr<IDXGISwapChain3> _swapchain;

    WRL::ComPtr<D3D12MA::Allocator> _memory_allocator;
    WRL::ComPtr<DescriptorPool> _srv_cbv_uav_pool;
    WRL::ComPtr<DescriptorPool> _sampler_pool;

    WRL::ComPtr<DescriptorPool> _shader_srv_cbv_uav_pool;
    WRL::ComPtr<DescriptorPool> _shader_sampler_pool;

    std::array<WRL::ComPtr<ID3D12CommandQueue>, 3> _queues;
    std::array<WRL::ComPtr<ID3D12Fence>, 3> _fences;
    std::array<uint64_t, 3> _fence_values;

    friend class Texture_D3D12;

    void create_back_buffers();
};

}  // namespace ionengine::renderer
