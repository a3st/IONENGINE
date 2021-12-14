// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/backend.h>

#define NOMINMAX

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(Result) if(FAILED(Result)) throw std::runtime_error("123");
#endif

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <platform/window.h>

using Microsoft::WRL::ComPtr;
using namespace ionengine::renderer;

D3D12_HEAP_TYPE d3d12_heap_type(const MemoryType type) {
	
	switch(type) {
        case MemoryType::Default: return D3D12_HEAP_TYPE_DEFAULT;
        case MemoryType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
        case MemoryType::Readback: return D3D12_HEAP_TYPE_READBACK;
		default: assert(false && "passed invalid argument to ToD3D12HeapType"); return D3D12_HEAP_TYPE_DEFAULT;
    }
}

struct Backend::Impl {
    ComPtr<IDXGIFactory4> factory;
    ComPtr<ID3D12Debug> debug;
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device4> device;
    ComPtr<IDXGISwapChain3> swapchain;

    struct {
        ComPtr<ID3D12CommandQueue> gfx;
        ComPtr<ID3D12CommandQueue> copy;
        ComPtr<ID3D12CommandQueue> compute;
    } queues;

    //ComPtr<ID3D12Resource> resources;
};

Backend::Backend(uint32_t const adapter_index, platform::Window* const window) : impl_(std::make_unique<Impl>()) {

    THROW_IF_FAILED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(impl_->debug.GetAddressOf())));
    impl_->debug->EnableDebugLayer();

    THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(impl_->factory.GetAddressOf())));

    THROW_IF_FAILED(impl_->factory->EnumAdapters1(adapter_index, impl_->adapter.GetAddressOf()));
    /*{
        DXGI_ADAPTER_DESC adapter_desc{};
        adapter_->GetDesc(&adapter_desc);

        size_t length = wcslen(adapter_desc.Description) + 1;
        assert(length > 0 && "length is less than 0 or equal 0");
        size_t result = 0;
        wcstombs_s(&result, adapter_desc_.name.data(), length, adapter_desc.Description, length - 1);
        
        adapter_desc_.local_memory = adapter_desc.DedicatedVideoMemory;
        adapter_desc_.vendor_id = adapter_desc.VendorId;
        adapter_desc_.device_id = adapter_desc.DeviceId;
    }*/

    THROW_IF_FAILED(D3D12CreateDevice(impl_->adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4), reinterpret_cast<void**>(impl_->device.GetAddressOf())));

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    THROW_IF_FAILED(impl_->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(impl_->queues.gfx.GetAddressOf())));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    THROW_IF_FAILED(impl_->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(impl_->queues.copy.GetAddressOf())));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    THROW_IF_FAILED(impl_->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(impl_->queues.compute.GetAddressOf())));

    platform::Size window_size = window->get_size();

    DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
    swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.Width = window_size.width;
    swapchain_desc.Height = window_size.height;
    swapchain_desc.BufferCount = 2;
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    THROW_IF_FAILED(impl_->factory->CreateSwapChainForHwnd(
        impl_->queues.gfx.Get(), 
        reinterpret_cast<HWND>(window->get_handle()), 
        &swapchain_desc, 
        nullptr, 
        nullptr, 
        reinterpret_cast<IDXGISwapChain1**>(impl_->swapchain.GetAddressOf()))
    );
}

Backend::~Backend() = default;

struct Buffer::Impl {
    ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC desc;
};

Buffer::Buffer() = default;

Buffer::Buffer(Backend& backend, BufferType const type, size_t const size) : impl_(std::make_unique<Impl>()) {

    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = 1024;
    resource_desc.Height = 1;
    resource_desc.MipLevels = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if(type == BufferType::Constant) {
        resource_desc.Width = (1024 + 255) & ~255;
    }

    impl_->desc = resource_desc;
}

Buffer::Buffer(Buffer&&) = default;

Buffer& Buffer::operator=(Buffer&&) = default;

Buffer::~Buffer() = default;

struct Memory::Impl {
    Backend* backend;
    ComPtr<ID3D12Heap> heap;
    ComPtr<ID3D12Resource> resource;
    MemoryType type;
};

Memory::Memory() = default;

Memory::Memory(Backend& backend, MemoryType const type, const size_t size) : impl_(std::make_unique<Impl>()) {

    impl_->type = type;
    impl_->backend = &backend;

    D3D12_HEAP_DESC heap_desc{};
    heap_desc.SizeInBytes = size;
    heap_desc.Properties.Type = d3d12_heap_type(type);
    THROW_IF_FAILED(backend.impl_->device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(impl_->heap.GetAddressOf())));

    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = 1024;
    resource_desc.Height = 1;
    resource_desc.MipLevels = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_RESOURCE_STATES initial_state{};
    switch(impl_->type) {
        case MemoryType::Default: initial_state = D3D12_RESOURCE_STATE_COMMON; break;
        case MemoryType::Readback:
        case MemoryType::Upload: initial_state = D3D12_RESOURCE_STATE_GENERIC_READ; break;
    }

    THROW_IF_FAILED(backend.impl_->device->CreatePlacedResource(
        impl_->heap.Get(), 
        0, 
        &resource_desc, 
        initial_state, 
        nullptr, 
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(impl_->resource.GetAddressOf())
    ));
}

Memory::Memory(Memory&&) = default;

Memory& Memory::operator=(Memory&&) = default;

MemoryType Memory::get_type() const {

    return impl_->type;
}

void Memory::bind_buffer(Buffer& buffer, uint64_t offset) {

    D3D12_RESOURCE_STATES initial_state;
    switch(impl_->type) {
        case MemoryType::Default: initial_state = D3D12_RESOURCE_STATE_COMMON; break;
        case MemoryType::Readback:
        case MemoryType::Upload: initial_state = D3D12_RESOURCE_STATE_GENERIC_READ; break;
    }
    
    THROW_IF_FAILED(impl_->backend->impl_->device->CreatePlacedResource(
        impl_->heap.Get(), 
        offset, 
        &buffer.impl_->desc, 
        initial_state, 
        nullptr, 
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(buffer.impl_->resource.GetAddressOf())
    ));
}

char8_t* Memory::map() {

    char8_t* data;
    D3D12_RANGE range{};
    THROW_IF_FAILED(impl_->resource->Map(0, &range, reinterpret_cast<void**>(&data)));
    return data;
}

void Memory::unmap() {
    
    D3D12_RANGE range{};
    impl_->resource->Unmap(0, &range);
}

Memory::~Memory() = default;

struct CommandBuffer::Impl {
    Backend* backend;
    ComPtr<ID3D12Resource> resource;
    CommandBufferType type;
};

CommandBuffer::CommandBuffer() = default;

CommandBuffer::CommandBuffer(Backend& backend, CommandBufferType const type) : impl_(std::make_unique<Impl>()) {

    impl_->type = type;
    impl_->backend = &backend;
  
}

CommandBuffer::CommandBuffer(CommandBuffer&&) = default;

CommandBuffer& CommandBuffer::operator=(CommandBuffer&&) = default;



CommandBuffer::~CommandBuffer() = default;