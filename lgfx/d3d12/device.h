// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

struct AdapterDesc {
    std::string name;
    uint32_t vendor_id;
    uint32_t device_id;
    size_t local_memory;
};

class Device {
    
friend class Fence;
friend class Texture;
friend class Buffer;
friend class MemoryHeap;
friend class DescriptorHeap;
friend class TextureView;
friend class BufferView;
friend class CommandBuffer;
friend class DescriptorLayout;
friend class Pipeline;

public:

    Device(const uint32_t adapter_index, void* hwnd, const uint32_t width, const uint32_t height, const uint32_t buffer_count, const uint32_t multisample_count);
    Device(const Device&) = delete;
    Device(Device&&) = delete;

    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;

    void Present();
    void Signal(const CommandBufferType type, Fence* fence, const uint64_t value);
    void Wait(const CommandBufferType type, Fence* fence, const uint64_t value);
    void ExecuteCommandBuffer(const CommandBufferType type, CommandBuffer* buffer);

    inline uint32_t GetSwapchainBufferIndex() const { return swapchain_->GetCurrentBackBufferIndex(); }
    inline const AdapterDesc& GetAdapterDesc() const { return adapter_desc_; }

private:

    AdapterDesc adapter_desc_;

    ComPtr<IDXGIFactory4> factory_;
    ComPtr<ID3D12Debug> debug_;
    ComPtr<IDXGIAdapter1> adapter_;
    ComPtr<ID3D12Device4> device_;
    ComPtr<IDXGISwapChain3> swapchain_;

    ComPtr<ID3D12CommandQueue> direct_queue_;
    ComPtr<ID3D12CommandQueue> copy_queue_;
    ComPtr<ID3D12CommandQueue> compute_queue_;

    uint32_t rtv_descriptor_offset_;
    uint32_t dsv_descriptor_offset_;
    uint32_t sampler_descriptor_offset_;
    uint32_t srv_descriptor_offset_;
};

}