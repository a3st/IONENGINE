// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/backend.h>
#include <renderer/handle_pool.h>
#include <platform/window.h>
#include <lib/exception.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#define NOMINMAX

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(Result) if(FAILED(Result)) throw ionengine::Exception(hresult_to_string(Result));
#endif

using Microsoft::WRL::ComPtr;
using namespace ionengine::renderer;

struct MemoryHeap {
    ComPtr<ID3D12Heap> heap;
    std::vector<uint8_t> blocks;
    uint64_t offset;
};

struct DescriptorHeap {
    ComPtr<ID3D12DescriptorHeap> heap;
    std::vector<uint8_t> blocks;
    uint32_t offset;
};

struct MemoryAllocInfo {
    MemoryHeap* heap;
    size_t size;
    uint64_t offset;
};

struct DescriptorAllocInfo {
    DescriptorHeap* heap;
    uint32_t offset;
};

std::u8string hresult_to_string(HRESULT const result) {

	switch(result) {
		case E_FAIL: return u8"Attempted to create a device with the debug layer enabled and the layer is not installed";
		case E_INVALIDARG: return u8"An invalid parameter was passed to the returning function";
		case E_OUTOFMEMORY: return u8"Direct3D could not allocate sufficient memory to complete the call";
		case E_NOTIMPL: return u8"The method call isn't implemented with the passed parameter combination";
		case S_FALSE: return u8"Alternate success value, indicating a successful but nonstandard completion";
		case S_OK: return u8"No error occurred";
		case D3D12_ERROR_ADAPTER_NOT_FOUND: return u8"The specified cached PSO was created on a different adapter and cannot be reused on the current adapter";
		case D3D12_ERROR_DRIVER_VERSION_MISMATCH: return u8"The specified cached PSO was created on a different driver version and cannot be reused on the current adapter";
		case DXGI_ERROR_INVALID_CALL: return u8"The method call is invalid. For example, a method's parameter may not be a valid pointer";
		case DXGI_ERROR_WAS_STILL_DRAWING: return u8"The previous blit operation that is transferring information to or from this surface is incomplete";
	}
    return u8"An unknown error has occurred";
}

class MemoryPool {
public:

    MemoryPool() = default;

    MemoryPool(MemoryPool const&) = delete;
    
    MemoryPool(MemoryPool&&) = default;

    MemoryPool& operator=(MemoryPool const&) = delete;

    MemoryPool& operator=(MemoryPool&&) noexcept = default;

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

    // TO DO
    return MemoryAllocInfo { nullptr, 0, 0 };
}

void MemoryPool::deallocate(MemoryAllocInfo const& alloc_info) {

    // TO DO
}

class DescriptorPool {
public:

    DescriptorPool() = default;

    DescriptorPool(DescriptorPool const&) = delete;

    DescriptorPool(DescriptorPool&&) noexcept = default;

    DescriptorPool& operator=(DescriptorPool const&) = delete;

    DescriptorPool& operator=(DescriptorPool&&) = default;

    DescriptorPool(ID3D12Device4* device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const heap_size);

    DescriptorAllocInfo allocate();
    void deallocate(DescriptorAllocInfo const& alloc_info);

private:

    std::list<DescriptorHeap> _heaps;
    uint32_t _heap_size;
};

DescriptorPool::DescriptorPool(ID3D12Device4* device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const heap_size) : _heap_size(heap_size) {

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
    heap_desc.NumDescriptors = heap_size;
    heap_desc.Type = heap_type;

    ComPtr<ID3D12DescriptorHeap> heap;
    THROW_IF_FAILED(device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(heap.GetAddressOf())));

    _heaps.emplace_back(
        DescriptorHeap { 
            heap, 
            std::vector<uint8_t>(heap_size, 0x0), 
            0 
        }
    );
}

DescriptorAllocInfo DescriptorPool::allocate() {

    DescriptorAllocInfo alloc_info{};

    for(auto& heap : _heaps) {
        if(heap.offset + 1 > _heap_size) {     
            continue;
        } else {
            for(uint32_t i = 0; i < heap.blocks.size(); ++i) {
                if(heap.blocks[i] == 0x0) {
                    alloc_info.heap = &heap;
                    alloc_info.offset = i;
                    heap.blocks[i] = 0x1;
                    ++heap.offset;

                    std::cout << "Allocated descriptor" << std::endl;
                    break;
                }
            }
        }
        if(alloc_info.heap) {
            break;
        }
    }

    if(!alloc_info.heap) {

    }

    return alloc_info;
}

void DescriptorPool::deallocate(DescriptorAllocInfo const& alloc_info) {

    alloc_info.heap->blocks[alloc_info.offset] = 0x0;
    --alloc_info.heap->offset;
}

struct Backend::Impl {
    ComPtr<IDXGIFactory4> factory;
    ComPtr<ID3D12Debug> debug;
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device4> device;
    ComPtr<IDXGISwapChain3> swapchain;

    struct {
        ComPtr<ID3D12CommandQueue> direct;
        ComPtr<ID3D12CommandQueue> copy;
        ComPtr<ID3D12CommandQueue> compute;
    } queues;

    struct {
        uint32_t rtv;
        uint32_t cbv_srv_uav;
        uint32_t sampler;
        uint32_t dsv;
    } descriptor_sizes;

    struct {
        MemoryPool default_tex;
        MemoryPool upload_stage;
    } mem_pools;

    struct {
        DescriptorPool rtv;
        DescriptorPool sampler;
        DescriptorPool srv;
        DescriptorPool dsv;
    } descriptor_pools;

    struct {
        using CommandListData = std::pair<ComPtr<ID3D12GraphicsCommandList4>, ComPtr<ID3D12CommandAllocator>>;
        HandlePool<CommandListData> cmds;

    } handles;

    /*using ImageData = std::pair<ComPtr<ID3D12Resource>, MemoryAllocInfo>;
    HandlePool<ImageData> images;

    HandlePool<DescriptorAllocInfo> image_views;

    uint32_t frame_index;
    uint32_t frame_count;

    using CommandData = std::pair<ComPtr<ID3D12GraphicsCommandList4>, ComPtr<ID3D12CommandAllocator>>;
    HandlePool<CommandData> cmds;*/
};

Backend::Backend(uint32_t const adapter_index, platform::Window* const window, uint32_t const frame_count) : _impl(std::make_unique<Impl>()) {

    THROW_IF_FAILED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(_impl->debug.GetAddressOf())));
    _impl->debug->EnableDebugLayer();

    THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(_impl->factory.GetAddressOf())));

    THROW_IF_FAILED(_impl->factory->EnumAdapters1(adapter_index, _impl->adapter.GetAddressOf()));
    
    /*{
        DXGI_ADAPTER_DESC adapter_desc{};
        _impl->adapter->GetDesc(&adapter_desc);

        size_t length = wcslen(adapter_desc.Description) + 1;
        assert(length > 0 && "length is less than 0 or equal 0");
        size_t result = 0;
        wcstombs_s(&result, adapter_desc_.name.data(), length, adapter_desc.Description, length - 1);
        
        adapter_desc_.local_memory = adapter_desc.DedicatedVideoMemory;
        adapter_desc_.vendor_id = adapter_desc.VendorId;
        adapter_desc_.device_id = adapter_desc.DeviceId;
    }*/

    THROW_IF_FAILED(D3D12CreateDevice(_impl->adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4), reinterpret_cast<void**>(_impl->device.GetAddressOf())));

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    THROW_IF_FAILED(_impl->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(_impl->queues.direct.GetAddressOf())));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    THROW_IF_FAILED(_impl->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(_impl->queues.copy.GetAddressOf())));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    THROW_IF_FAILED(_impl->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(_impl->queues.compute.GetAddressOf())));

    //_impl->device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE)

    _impl->descriptor_sizes.rtv = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _impl->descriptor_sizes.cbv_srv_uav = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _impl->descriptor_sizes.dsv = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    _impl->descriptor_sizes.sampler = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    _impl->descriptor_pools.rtv = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 64);
    _impl->descriptor_pools.dsv = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 16);
    _impl->descriptor_pools.sampler = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 64);
    _impl->descriptor_pools.srv = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128);
    
    _impl->mem_pools.default_tex = MemoryPool(_impl->device.Get(), D3D12_HEAP_TYPE_DEFAULT, 1024 * 1024 * 256);
    _impl->mem_pools.upload_stage = MemoryPool(_impl->device.Get(), D3D12_HEAP_TYPE_UPLOAD, 1024 * 1024 * 64);
    
    platform::Size window_size = window->get_size();
    //_impl->frame_count = frame_count;

    DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
    swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.Width = window_size.width;
    swapchain_desc.Height = window_size.height;
    swapchain_desc.BufferCount = frame_count;
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    THROW_IF_FAILED(_impl->factory->CreateSwapChainForHwnd(
        _impl->queues.direct.Get(), 
        reinterpret_cast<HWND>(window->get_handle()), 
        &swapchain_desc, 
        nullptr, 
        nullptr, 
        reinterpret_cast<IDXGISwapChain1**>(_impl->swapchain.GetAddressOf()))
    );

    /*impl_->swapchain_images.reserve(frame_count);
    impl_->swapchain_views.reserve(frame_count);

    impl_->default_pool = std::make_unique<MemoryPool>(impl_->device.Get(), D3D12_HEAP_TYPE_DEFAULT, 1024 * 1024 * 256);
    impl_->rtv_pool = std::make_unique<DescriptorPool>(impl_->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 256);

    for(uint32_t i = 0; i < frame_count; ++i) {
        ComPtr<ID3D12Resource> resource;
        THROW_IF_FAILED(impl_->swapchain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource.GetAddressOf())));

        D3D12_RENDER_TARGET_VIEW_DESC view_desc{};
        view_desc.Format = swapchain_desc.Format;
        view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        DescriptorAllocInfo descriptor_alloc_info = impl_->rtv_pool->allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + descriptor_alloc_info.offset * impl_->descriptor_sizes.rtv };
        impl_->device->CreateRenderTargetView(resource.Get(), &view_desc, cpu_handle);

        impl_->swapchain_images.emplace_back(impl_->images.push({ resource, {} }) - 1);
        impl_->swapchain_views.emplace_back(impl_->image_views.push(descriptor_alloc_info) - 1);
    }
*/
    /*impl_->write_cmds_index = {};

    for(uint32_t i = 0; i < 10; ++i) {
        ComPtr<ID3D12CommandAllocator> allocator;
        THROW_IF_FAILED(impl_->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(allocator.GetAddressOf())));
    
        ComPtr<ID3D12GraphicsCommandList4> cmd;
        THROW_IF_FAILED(impl_->device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, __uuidof(ID3D12GraphicsCommandList4), reinterpret_cast<void**>(cmd.GetAddressOf())));
    
        impl_->write_cmds.direct.emplace_back(impl_->cmds.push({ cmd, allocator }) - 1);
    }*/
}

Backend::~Backend() = default;

GPUResourceHandle Backend::generate_command_buffer(RenderQueue const& queue) {

    return GPUResourceHandle { };
}

void Backend::execute_command_buffers(std::vector<GPUResourceHandle> const& handles) {

}

void Backend::swap_buffers() {

}

void Backend::resize_buffers(uint32_t const width, uint32_t const height, uint32_t frame_count) {

}

/*ImageId Backend::create_image(
    ImageDimension const dimension, 
    uint32_t const width, 
    uint32_t const height, 
    uint16_t const mip_levels, 
    uint16_t const array_layers,
    ImageFormat const format,
    ImageFlags const flags
) {

    D3D12_RESOURCE_DESC resource_desc{};
    switch(dimension) {
        case ImageDimension::_1D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D; break;
        case ImageDimension::_2D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; break;
        case ImageDimension::_3D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; break;
    }
    resource_desc.Width = width;
    resource_desc.Height = height;
    resource_desc.MipLevels = mip_levels;
    resource_desc.DepthOrArraySize = array_layers;
    resource_desc.SampleDesc.Count = 1;
    switch(format) {
        case ImageFormat::Unknown: resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        case ImageFormat::RGBA8Unorm: resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    }
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

std::pair<ImageId, ImageViewId> Backend::acquire_swapchain_attachment() {

    impl_->frame_index = (impl_->frame_index + 1) % impl_->frame_count;
    return { impl_->swapchain_images[impl_->frame_index], impl_->swapchain_views[impl_->frame_index] };
}

CommandBufferId Backend::write_cmd(CommandBufferType const cmd_type, std::vector<CommandBufferEvent> const& events) {

    CommandBufferId buffer_id;

    switch(cmd_type) {
        case CommandBufferType::Graphics: {
            buffer_id = impl_->write_cmds.direct[impl_->write_cmds_index.direct];
            ++impl_->write_cmds_index.direct;
        } break;
    }

    return buffer_id;
}

FenceId Backend::execute(CommandBufferType const cmd_type, std::vector<CommandBufferId> const& buffers) {
    return { 0 };
}*/