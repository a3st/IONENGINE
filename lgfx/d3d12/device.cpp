// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "device.h"
#include "fence.h"

using namespace lgfx;

Device::Device() {

}

Device::~Device() {

    compute_queue_->Release();
    copy_queue_->Release();
    direct_queue_->Release();
    swapchain_->Release();
    device_->Release();
    adapter_->Release();
    debug_->Release();
    factory_->Release();
}

Device::Device(const uint32_t adapter_index, void* hwnd, const uint32_t width, const uint32_t height, const uint32_t buffer_count, const uint32_t multisample_count) {

    assert(hwnd && "invalid pointer to hwnd");

    THROW_IF_FAILED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(&debug_)));
    debug_->EnableDebugLayer();

    THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(&factory_)));
    THROW_IF_FAILED(factory_->EnumAdapters1(adapter_index, &adapter_));
    {
        DXGI_ADAPTER_DESC adapter_desc{};
        adapter_->GetDesc(&adapter_desc);

        size_t length = wcslen(adapter_desc.Description) + 1;
        assert(length > 0 && "length is less than 0 or equal 0");
        size_t result = 0;
        wcstombs_s(&result, adapter_desc_.name.data(), length, adapter_desc.Description, length - 1);
        
        adapter_desc_.local_memory = adapter_desc.DedicatedVideoMemory;
        adapter_desc_.vendor_id = adapter_desc.VendorId;
        adapter_desc_.device_id = adapter_desc.DeviceId;
    }

    THROW_IF_FAILED(D3D12CreateDevice(adapter_, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), reinterpret_cast<void**>(&device_)));

    rtv_descriptor_offset_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    dsv_descriptor_offset_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    sampler_descriptor_offset_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    srv_descriptor_offset_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    THROW_IF_FAILED(device_->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(direct_queue_)));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    THROW_IF_FAILED(device_->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(copy_queue_)));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    THROW_IF_FAILED(device_->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(compute_queue_)));

    DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
    swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.Width = width;
    swapchain_desc.Height = height;
    swapchain_desc.BufferCount = buffer_count;
    swapchain_desc.SampleDesc.Count = multisample_count;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    THROW_IF_FAILED(factory_->CreateSwapChainForHwnd(direct_queue_, reinterpret_cast<HWND>(hwnd), &swapchain_desc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapchain_)));
}

Device::Device(Device&& rhs) noexcept {

    adapter_desc_ = std::move(rhs.adapter_desc_);
    
    factory_ = rhs.factory_;
    debug_ = rhs.debug_;
    adapter_ = rhs.adapter_;
    device_ = rhs.device_;
    swapchain_ = rhs.swapchain_;

    direct_queue_ = rhs.direct_queue_;
    copy_queue_ = rhs.copy_queue_;
    compute_queue_ = rhs.compute_queue_;

    rtv_descriptor_offset_ = rhs.rtv_descriptor_offset_;
    dsv_descriptor_offset_ = rhs.dsv_descriptor_offset_;
    sampler_descriptor_offset_ = rhs.sampler_descriptor_offset_;
    srv_descriptor_offset_ = rhs.srv_descriptor_offset_;
}

Device& Device::operator=(Device&& rhs) noexcept {

    adapter_desc_ = std::move(rhs.adapter_desc_);
    
    factory_ = rhs.factory_;
    debug_ = rhs.debug_;
    adapter_ = rhs.adapter_;
    device_ = rhs.device_;
    swapchain_ = rhs.swapchain_;

    direct_queue_ = rhs.direct_queue_;
    copy_queue_ = rhs.copy_queue_;
    compute_queue_ = rhs.compute_queue_;

    rtv_descriptor_offset_ = rhs.rtv_descriptor_offset_;
    dsv_descriptor_offset_ = rhs.dsv_descriptor_offset_;
    sampler_descriptor_offset_ = rhs.sampler_descriptor_offset_;
    srv_descriptor_offset_ = rhs.srv_descriptor_offset_;
    return *this;
}

void Device::Present() {
    
    THROW_IF_FAILED(swapchain_->Present(0, 0));
}

void Device::Signal(const CommandBufferType type, Fence* fence, const uint64_t value) {
    
    switch(type) {
        case CommandBufferType::kGraphics: {
            THROW_IF_FAILED(direct_queue_->Signal(fence->fence_, value));
            break;
        }
        case CommandBufferType::kCopy: {
            THROW_IF_FAILED(copy_queue_->Signal(fence->fence_, value));
            break;
        }
        case CommandBufferType::kCompute: {
            THROW_IF_FAILED(compute_queue_->Signal(fence->fence_, value));
            break;
        }
    }
}

void Device::Wait(const CommandBufferType type, Fence* fence, const uint64_t value) {

    switch(type) {
        case CommandBufferType::kGraphics: {
            THROW_IF_FAILED(direct_queue_->Wait(fence->fence_, value));
            break;
        }
        case CommandBufferType::kCopy: {
            THROW_IF_FAILED(copy_queue_->Wait(fence->fence_, value));
            break;
        }
        case CommandBufferType::kCompute: {
            THROW_IF_FAILED(compute_queue_->Wait(fence->fence_, value));
            break;
        }
    }
}