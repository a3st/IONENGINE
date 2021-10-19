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
friend struct MemoryHeap;
friend struct DescriptorHeap;
friend class TextureView;
friend class CommandBuffer;

public:

    Device();
    ~Device();
    Device(const uint32_t adapter_index, void* hwnd, const uint32_t width, const uint32_t height, const uint32_t buffer_count, const uint32_t multisample_count);
    Device(const Device&) = delete;
    Device(Device&& rhs) noexcept;
    
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&& rhs) noexcept;

    void Present();
    void Signal(const CommandBufferType type, Fence* fence, const uint64_t value);
    void Wait(const CommandBufferType type, Fence* fence, const uint64_t value);
    void ExecuteCommandBuffer(const CommandBufferType type, CommandBuffer* buffer);

    inline const AdapterDesc& GetAdapterDesc() const { return adapter_desc_; }

private:

    AdapterDesc adapter_desc_;

    IDXGIFactory4* factory_;
    ID3D12Debug* debug_;
    IDXGIAdapter1* adapter_;
    ID3D12Device4* device_;
    IDXGISwapChain3* swapchain_;

    ID3D12CommandQueue* direct_queue_;
    ID3D12CommandQueue* copy_queue_;
    ID3D12CommandQueue* compute_queue_;

    uint32_t rtv_descriptor_offset_;
    uint32_t dsv_descriptor_offset_;
    uint32_t sampler_descriptor_offset_;
    uint32_t srv_descriptor_offset_;
};

}