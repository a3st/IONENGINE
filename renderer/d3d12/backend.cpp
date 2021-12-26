// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/backend.h>
#include <renderer/handle_pool.h>

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

struct MemoryHeap {
    ComPtr<ID3D12Heap> heap;
    std::vector<uint8_t> blocks;
    uint64_t offset;
};

struct MemoryAllocInfo {
    MemoryHeap* heap;
    size_t size;
    uint64_t offset;
};

class MemoryPool {
public:

    MemoryPool(ID3D12Device4* device, D3D12_HEAP_TYPE const heap_type, size_t const heap_size);

    MemoryAllocInfo allocate(size_t const size);
    void deallocate(MemoryAllocInfo const& alloc_info);

private:

    std::list<MemoryHeap> _heaps;
};

MemoryPool::MemoryPool(ID3D12Device4* device, D3D12_HEAP_TYPE const heap_type, size_t const heap_size) {

    D3D12_HEAP_DESC heap_desc{};
    heap_desc.SizeInBytes = heap_size;
    heap_desc.Properties.Type = heap_type;

    ComPtr<ID3D12Heap> heap;
    THROW_IF_FAILED(device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(heap.GetAddressOf())));

    _heaps.emplace_back(MemoryHeap { heap, std::vector<uint8_t>(heap_size / 1048576, 0x0), 0 });
}

MemoryAllocInfo MemoryPool::allocate(size_t const size) {

    std::cout << "123 allocated" << std::endl;
    return MemoryAllocInfo { nullptr, 0, 0 };
}

void MemoryPool::deallocate(MemoryAllocInfo const& alloc_info) {

    std::cout << "123 deallocated" << std::endl;
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

    std::unique_ptr<MemoryPool> default_pool;

    using ImageData = std::pair<ComPtr<ID3D12Resource>, MemoryAllocInfo>;
    HandlePool<ImageData> images;
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

    impl_->default_pool = std::make_unique<MemoryPool>(impl_->device.Get(), D3D12_HEAP_TYPE_DEFAULT, 1024 * 1024 * 256);
}

ImageId Backend::create_image(
    ImageDimension const dimension, 
    uint32_t const width, 
    uint32_t const height, 
    uint16_t const mip_levels, 
    uint16_t const array_layers,
    ImageFormat const format,
    ImageFlags const flags
) {

    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = width;
    resource_desc.Height = height;
    resource_desc.MipLevels = mip_levels;
    resource_desc.DepthOrArraySize = array_layers;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ComPtr<ID3D12Resource> resource;

    D3D12_HEAP_PROPERTIES heap_props{};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    impl_->device->CreateCommittedResource(
        &heap_props, 
        D3D12_HEAP_FLAG_NONE, 
        &resource_desc, 
        D3D12_RESOURCE_STATE_COMMON, 
        nullptr, 
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(resource.GetAddressOf())
    );

    D3D12_RESOURCE_ALLOCATION_INFO res_alloc_info = impl_->device->GetResourceAllocationInfo(0, 1, &resource_desc);
    MemoryAllocInfo mem_alloc_info = impl_->default_pool->allocate(res_alloc_info.SizeInBytes + res_alloc_info.Alignment);

    return ImageId(impl_->images.push({ resource, mem_alloc_info }) - 1);
}

void Backend::free_image(ImageId const& image_id) {

    impl_->default_pool->deallocate(impl_->images[image_id.id()].second);
    impl_->images[image_id.id()].first = nullptr;
    impl_->images.erase(image_id.id());
}

Backend::~Backend() = default;