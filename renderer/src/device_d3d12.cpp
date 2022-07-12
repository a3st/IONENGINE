// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/impl/device_d3d12.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

core::Expected<std::unique_ptr<Device>, std::string> Device_D3D12::create(
    platform::Window& window, uint16_t const sample_count) noexcept {
    auto device = std::make_unique<Device_D3D12>();

    // Initialize class members
    uint32_t factory_flags = 0;
    HRESULT result;
#ifdef _DEBUG
    result = D3D12GetDebugInterface(
        __uuidof(ID3D12Debug),
        reinterpret_cast<void**>(device->_debug.GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    device->_debug->EnableDebugLayer();
#endif

    // Initialize DXGI factory for D3D12 functionals
    result = CreateDXGIFactory2(
        factory_flags, __uuidof(IDXGIFactory4),
        reinterpret_cast<void**>(device->_factory.GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    // Select high performance adapter for device
    result = device->_factory->EnumAdapterByGpuPreference(
        0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter1),
        reinterpret_cast<void**>(device->_adapter.GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    // Get adapter information
    DXGI_ADAPTER_DESC1 adapter_desc;
    result = device->_adapter->GetDesc1(&adapter_desc);

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    // Adapter information TODO

    // Initialize D3D12 device with feature level 12_0
    result = D3D12CreateDevice(
        device->_adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4),
        reinterpret_cast<void**>(device->_device.GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    // Initialize device queues and fences
    std::fill(device->_fence_values.begin(), device->_fence_values.end(), 1);

    // Initialize graphics queue
    D3D12_COMMAND_QUEUE_DESC command_queue_desc = {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL};

    result = device->_device->CreateCommandQueue(
        &command_queue_desc, __uuidof(ID3D12CommandQueue),
        reinterpret_cast<void**>(device->_queues[0].GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    result = device->_device->CreateFence(
        0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence),
        reinterpret_cast<void**>(device->_fences[0].GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    // Initialize copy queue
    command_queue_desc = {.Type = D3D12_COMMAND_LIST_TYPE_COPY,
                          .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL};

    result = device->_device->CreateCommandQueue(
        &command_queue_desc, __uuidof(ID3D12CommandQueue),
        reinterpret_cast<void**>(device->_queues[1].GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    result = device->_device->CreateFence(
        0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence),
        reinterpret_cast<void**>(device->_fences[1].GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    // Initialize compute queue
    command_queue_desc = {.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE,
                          .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL};

    result = device->_device->CreateCommandQueue(
        &command_queue_desc, __uuidof(ID3D12CommandQueue),
        reinterpret_cast<void**>(device->_queues[2].GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    result = device->_device->CreateFence(
        0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence),
        reinterpret_cast<void**>(device->_fences[2].GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    D3D12MA::ALLOCATOR_DESC allocator_desc = {
        .pDevice = device->_device.Get(), .pAdapter = device->_adapter.Get()};

    result = D3D12MA::CreateAllocator(&allocator_desc,
                                      device->_memory_allocator.GetAddressOf());

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<Device>, std::string>(
            to_string(result));
    }

    return core::make_expected<std::unique_ptr<Device>, std::string>(
        std::move(device));
}

core::Expected<std::unique_ptr<Device>, std::string> Device::create(
    platform::Window& window, uint16_t const sample_count) noexcept {
    return Device_D3D12::create(window, sample_count);
}