// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/backend.h>
#include <lib/instance_container.h>
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

struct Texture {
    ComPtr<ID3D12Resource> resource;
    MemoryAllocInfo memory_alloc_info;
    DescriptorAllocInfo descriptor_alloc_info;
};

struct Buffer {
    ComPtr<ID3D12Resource> resource;
    MemoryAllocInfo memory_alloc_info;
    DescriptorAllocInfo descriptor_alloc_info;
};

struct Pipeline {

    ComPtr<ID3D12PipelineState> pipeline_state;
};

struct Shader {
    std::vector<char8_t> bytecode;
    D3D12_SHADER_VISIBILITY type;
};

struct RenderPass {
    std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> colors;
    std::optional<D3D12_RENDER_PASS_DEPTH_STENCIL_DESC> depth_stencil;
};

struct Backend::Impl {
    ComPtr<IDXGIFactory4> factory;
    ComPtr<ID3D12Debug> debug;
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device4> device;
    ComPtr<IDXGISwapChain3> swapchain;
    std::vector<ComPtr<ID3D12Fence>> fences;
    std::vector<ComPtr<ID3D12RootSignature>> signatures;

    HANDLE wait_event;

    ComPtr<ID3D12CommandQueue> direct_queue;
    ComPtr<ID3D12CommandQueue> copy_queue;
    ComPtr<ID3D12CommandQueue> compute_queue;


    struct {
        uint32_t rtv;
        uint32_t cbv_srv_uav;
        uint32_t sampler;
        uint32_t dsv;
    } descriptor_sizes;

    struct {
        MemoryPool default_tex;
        MemoryPool default_buffer;
        MemoryPool upload_stage;
    } mem_pools;

    struct {
        DescriptorPool rtv;
        DescriptorPool sampler;
        DescriptorPool srv;
        DescriptorPool dsv;
    } descriptor_pools;

    //using CommandListData = std::pair<ComPtr<ID3D12GraphicsCommandList4>, ComPtr<ID3D12CommandAllocator>>;

    InstanceContainer<Texture> textures;
    InstanceContainer<Buffer> buffers;
    InstanceContainer<Pipeline> pipelines;
    InstanceContainer<Shader> shaders;
    InstanceContainer<RenderPass> render_passes;

    std::map<std::vector<ShaderBindDesc>, uint16_t> signature_cache;

    std::vector<uint16_t> swap_indices;
    uint32_t frame_index;
    uint32_t frame_count;

    std::vector<uint64_t> fence_values;
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
    _impl->frame_count = frame_count;

    auto swapchain_desc = DXGI_SWAP_CHAIN_DESC1 {};
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

    _impl->swap_indices.reserve(frame_count);
    _impl->fences.resize(frame_count);
    _impl->fence_values.resize(frame_count);

    for(uint32_t i = 0; i < frame_count; ++i) {
        ComPtr<ID3D12Resource> resource;
        THROW_IF_FAILED(_impl->swapchain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource.GetAddressOf())));

        auto view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
        view_desc.Format = swapchain_desc.Format;
        view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        DescriptorAllocInfo descriptor_alloc_info = _impl->descriptor_pools.rtv.allocate();

        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _impl->descriptor_sizes.rtv * // Device descriptor size
            descriptor_alloc_info.offset // Allocation offset
        };
        _impl->device->CreateRenderTargetView(resource.Get(), &view_desc, cpu_handle);

        _impl->swap_indices.emplace_back(
            static_cast<uint16_t>(_impl->handles.textures.push(
                Backend::Impl::TextureData {
                    resource,
                    {},
                    descriptor_alloc_info,
                    true
                }
            ))
        ); // Add swap indices to vector. Using it for get swap textures in future

        THROW_IF_FAILED(_impl->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(_impl->fences[i].GetAddressOf())));

        ++_impl->fence_values[i];
    }

    _impl->wait_event = CreateEvent(nullptr, false, false, nullptr);
    if(!_impl->wait_event) {
        THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
    }
}

Backend::~Backend() = default;

GPUResourceHandle Backend::create_texture(
    TextureDimension const dimension,
    Extent2D const extent,
    uint16_t const mip_levels,
    uint16_t const array_layers,
    Format const format,
    TextureFlags const flags,
    TextureViewDesc const& texture_view
) {
    auto resource_desc = D3D12_RESOURCE_DESC {};
    switch(dimension) {
        case TextureDimension::_1D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D; break;
        case TextureDimension::_2D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; break;
        case TextureDimension::_3D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; break;
    }
    resource_desc.Width = extent.width;
    resource_desc.Height = extent.height;
    resource_desc.MipLevels = mip_levels;
    resource_desc.DepthOrArraySize = array_layers;
    resource_desc.SampleDesc.Count = 1;
    switch(format) {
        case Format::Unknown: resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        case Format::RGBA8Unorm: resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_RESOURCE_ALLOCATION_INFO res_alloc_info = _impl->device->GetResourceAllocationInfo(0, 1, &resource_desc);
    MemoryAllocInfo mem_alloc_info = _impl->mem_pools.default_tex.allocate(res_alloc_info.SizeInBytes + res_alloc_info.Alignment);

    ComPtr<ID3D12Resource> resource;
    THROW_IF_FAILED(_impl->device->CreatePlacedResource(
        mem_alloc_info.heap->heap.Get(),
        mem_alloc_info.offset,
        &resource_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(resource.GetAddressOf())
    ));

    auto descriptor_alloc_info = DescriptorAllocInfo {};
    switch(flags) {
        case TextureFlags::RenderTarget: {
            auto view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
            view_desc.Format = resource_desc.Format;
            switch(dimension) {
                case TextureDimension::_1D: {
                    view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                    view_desc.Texture1D.MipSlice = mip_levels;
                } break;
                case TextureDimension::_2D: {
                    view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                    view_desc.Texture2D.MipSlice = mip_levels;
                    view_desc.Texture2D.PlaneSlice = array_layers;
                } break;
                case TextureDimension::_3D: {
                    view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                    view_desc.Texture3D.MipSlice = mip_levels;
                    view_desc.Texture3D.WSize = array_layers;
                } break;
            }

            descriptor_alloc_info = _impl->descriptor_pools.rtv.allocate();

            auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                _impl->descriptor_sizes.rtv * // Device descriptor size
                descriptor_alloc_info.offset // Allocation offset
            };
            _impl->device->CreateRenderTargetView(resource.Get(), &view_desc, cpu_handle);
        } break;
        
        case TextureFlags::DepthStencil: {
            auto view_desc = D3D12_DEPTH_STENCIL_VIEW_DESC {};
            view_desc.Format = resource_desc.Format;
            switch(dimension) {
                case TextureDimension::_1D: {
                    view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
                    view_desc.Texture1D.MipSlice = mip_levels;
                } break;
                case TextureDimension::_2D: {
                    view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    view_desc.Texture2D.MipSlice = mip_levels;
                } break;
                default: {
                    assert(false && "Depth stencil view dimension is unsupported");
                } break;
            }

            descriptor_alloc_info = _impl->descriptor_pools.dsv.allocate();

            auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                _impl->descriptor_sizes.dsv * // Device descriptor size
                descriptor_alloc_info.offset // Allocation offset
            };
            _impl->device->CreateDepthStencilView(resource.Get(), &view_desc, cpu_handle);
        } break;

        case TextureFlags::UnorderedAccess: {
            // TO DO
        } break;

        default: {
            auto view_desc = D3D12_SHADER_RESOURCE_VIEW_DESC {};
            view_desc.Format = resource_desc.Format;
            switch(dimension) {
                case TextureDimension::_1D: {
                    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                    view_desc.Texture1D.MipLevels = mip_levels;
                } break;
                case TextureDimension::_2D: {
                    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    view_desc.Texture2D.MipLevels = mip_levels;
                    view_desc.Texture2D.PlaneSlice = array_layers;
                } break;
                case TextureDimension::_3D: {
                    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                    view_desc.Texture3D.MipLevels = mip_levels;
                } break;
            }
            view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            descriptor_alloc_info = _impl->descriptor_pools.srv.allocate();

            auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                _impl->descriptor_sizes.cbv_srv_uav * // Device descriptor size
                descriptor_alloc_info.offset // Allocation offset
            };
            _impl->device->CreateShaderResourceView(resource.Get(), &view_desc, cpu_handle);
        } break;
    }

    size_t handle_id = _impl->handles.textures.push(
        Backend::Impl::TextureData {
            resource,
            mem_alloc_info,
            descriptor_alloc_info
        }
    );
    return GPUResourceHandle(GPUResourceType::Texture, static_cast<uint16_t>(handle_id));
}

GPUResourceHandle Backend::create_buffer(
    size_t const size,
    BufferFlags const flags,
    BufferViewDesc const& buffer_view
) {
    auto resource_desc = D3D12_RESOURCE_DESC {};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = size;
    resource_desc.Height = 1;
    resource_desc.MipLevels = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_RESOURCE_ALLOCATION_INFO res_alloc_info = _impl->device->GetResourceAllocationInfo(0, 1, &resource_desc);
    MemoryAllocInfo mem_alloc_info = _impl->mem_pools.default_buffer.allocate(res_alloc_info.SizeInBytes + res_alloc_info.Alignment);

    ComPtr<ID3D12Resource> resource;
    THROW_IF_FAILED(_impl->device->CreatePlacedResource(
        mem_alloc_info.heap->heap.Get(),
        mem_alloc_info.offset,
        &resource_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(resource.GetAddressOf())
    ));

    auto descriptor_alloc_info = DescriptorAllocInfo {};
    switch(flags) {
        case BufferFlags::ConstantBuffer: {
            auto view_desc = D3D12_CONSTANT_BUFFER_VIEW_DESC {};
            view_desc.BufferLocation = resource->GetGPUVirtualAddress();
            view_desc.SizeInBytes = static_cast<uint32_t>(size);

            descriptor_alloc_info = _impl->descriptor_pools.srv.allocate();

            auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                _impl->descriptor_sizes.cbv_srv_uav * // Device descriptor size
                descriptor_alloc_info.offset // Allocation offset
            };
            _impl->device->CreateConstantBufferView(&view_desc, cpu_handle);
        } break;
    }

    size_t handle_id = _impl->handles.buffers.push(
        Backend::Impl::BufferData {
            resource,
            mem_alloc_info,
            descriptor_alloc_info
        }
    );
    return GPUResourceHandle(GPUResourceType::Buffer, static_cast<uint16_t>(handle_id));
}

GPUResourceHandle Backend::create_render_pass(
    std::vector<GPUResourceHandle> const& rtv_handles,
    std::vector<RenderPassColorDesc> const& rtv_ops,
    GPUResourceHandle const& dsv_handle,
    RenderPassDepthStencilDesc const& dsv_op
) {

    auto get_render_pass_begin_type = [&](RenderPassLoadOp const load_op) {
        switch (load_op) {
    	    case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	    case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		    case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    };
    
    auto get_render_pass_end_type = [&](RenderPassStoreOp const store_op) {
        switch (store_op) {
    	    case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	    case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    };

    std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> rtvs; 
    rtvs.resize(rtv_handles.size());

    for(size_t i = 0; i < rtvs.size(); ++i) {
        auto& texture_data = _impl->handles.textures[rtv_handles[i]._id & 0xffff];

        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            texture_data.descriptor_alloc.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _impl->descriptor_sizes.rtv * // Device descriptor size
            texture_data.descriptor_alloc.offset // Allocation offset
        };

        auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        begin.Type = get_render_pass_begin_type(rtv_ops[i].load_op);
        begin.Clear.ClearValue.Format = texture_data.resource->GetDesc().Format;

        auto end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        end.Type = get_render_pass_end_type(rtv_ops[i].store_op);

        rtvs[i].BeginningAccess = begin;
        rtvs[i].EndingAccess = end;
        rtvs[i].cpuDescriptor = cpu_handle;
    }

    std::optional<D3D12_RENDER_PASS_DEPTH_STENCIL_DESC> dsv_opt;

    if(dsv_handle != GPUResourceHandle()) {
        auto& texture_data = _impl->handles.textures[dsv_handle._id & 0xffff];

        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            texture_data.descriptor_alloc.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _impl->descriptor_sizes.dsv * // Device descriptor size
            texture_data.descriptor_alloc.offset // Allocation offset
        };

        auto depth_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        depth_begin.Type = get_render_pass_begin_type(dsv_op.depth_load_op);
        depth_begin.Clear.ClearValue.Format = texture_data.resource->GetDesc().Format;

        auto stencil_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        stencil_begin.Type = get_render_pass_begin_type(dsv_op.stencil_load_op);
        stencil_begin.Clear.ClearValue.Format = texture_data.resource->GetDesc().Format;

        auto depth_end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        depth_end.Type = get_render_pass_end_type(dsv_op.depth_store_op);

        auto stencil_end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        stencil_end.Type = get_render_pass_end_type(dsv_op.stencil_store_op);

        auto dsv = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC {};
        dsv.DepthBeginningAccess = depth_begin;
        dsv.StencilBeginningAccess = stencil_begin;
        dsv.DepthEndingAccess = depth_end;
        dsv.StencilEndingAccess = stencil_end;
        dsv.cpuDescriptor = cpu_handle;

        dsv_opt = dsv;
    }

    size_t handle_id = _impl->handles.render_passes.push(
        Backend::Impl::RenderPassData {
            rtvs,
            dsv_opt
        }
    );
    return GPUResourceHandle(GPUResourceType::RenderPass, static_cast<uint16_t>(handle_id));
}

GPUResourceHandle Backend::create_shader(
    std::span<char8_t> const shader_data,
    ShaderFlags const flags
) {

    std::vector<char8_t> data(shader_data.size());
    std::copy(shader_data.begin(), shader_data.end(), data.begin());
    
    D3D12_SHADER_VISIBILITY visibility;
    switch(flags) {
        case ShaderFlags::Vertex: visibility = D3D12_SHADER_VISIBILITY_VERTEX; break;
        case ShaderFlags::Pixel: visibility = D3D12_SHADER_VISIBILITY_VERTEX; break;
        case ShaderFlags::Geometry: visibility = D3D12_SHADER_VISIBILITY_GEOMETRY; break;
        case ShaderFlags::Domain: visibility = D3D12_SHADER_VISIBILITY_DOMAIN; break;
        case ShaderFlags::Hull: visibility = D3D12_SHADER_VISIBILITY_HULL; break;
        default: assert(false && "Shader visibility is unsupported"); break;
    }

    size_t handle_id = _impl->handles.shaders.push(
        Backend::Impl::ShaderData {
            data,
            visibility
        }
    );
    return GPUResourceHandle(GPUResourceType::Shader, static_cast<uint16_t>(handle_id));
}

GPUResourceHandle Backend::create_pipeline(
    std::vector<ShaderBindDesc> const& shader_bindings,
    std::vector<VertexInputDesc> const& vertex_inputs,
    std::vector<GPUResourceHandle> const& shader_handles,
    RasterizerDesc const& rasterizer,
    DepthStencilDesc const& depth_stencil,
    BlendDesc const& blend,
    GPUResourceHandle const& render_pass_handle
) {
    uint16_t signature_index;
    auto it = _impl->signature_cache.find(shader_bindings);
    if(it != _impl->signature_cache.end()) {
        signature_index = it->second;
    } else {
        std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
        ranges.reserve(shader_bindings.size());

        std::vector<D3D12_ROOT_PARAMETER> parameters;
        parameters.reserve(shader_bindings.size());

        auto get_descriptor_range_type = [&](ShaderBindType const bind_type) -> D3D12_DESCRIPTOR_RANGE_TYPE {
            switch(bind_type) {
                case ShaderBindType::ShaderResource: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                case ShaderBindType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                case ShaderBindType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                case ShaderBindType::UnorderedAccess: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            }
            return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        };

        auto get_shader_visibility = [&](ShaderFlags const shader_flags) -> D3D12_SHADER_VISIBILITY {
            switch(shader_flags) {
                case ShaderFlags::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
                case ShaderFlags::Pixel: return D3D12_SHADER_VISIBILITY_VERTEX;
                case ShaderFlags::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
                case ShaderFlags::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
                case ShaderFlags::Hull: return D3D12_SHADER_VISIBILITY_HULL;
                case ShaderFlags::All: return D3D12_SHADER_VISIBILITY_ALL;
            }
            return D3D12_SHADER_VISIBILITY_ALL;
        };

        for(auto& binding : shader_bindings) {
            auto range = D3D12_DESCRIPTOR_RANGE {};
            range.RangeType = get_descriptor_range_type(binding.bind_type);
            range.NumDescriptors = binding.count;
            range.BaseShaderRegister = binding.index;
            range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            ranges.emplace_back(range);

            auto parameter = D3D12_ROOT_PARAMETER {};
            parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameter.DescriptorTable.pDescriptorRanges = &ranges.back();
            parameter.DescriptorTable.NumDescriptorRanges = 1;
            parameter.ShaderVisibility = get_shader_visibility(binding.shader_flags);

            parameters.emplace_back(parameter);
        }

        auto root_desc = D3D12_ROOT_SIGNATURE_DESC {};
        root_desc.pParameters = parameters.data();
        root_desc.NumParameters = static_cast<uint32_t>(parameters.size());
        root_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> serialized_data;
        THROW_IF_FAILED(D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, serialized_data.GetAddressOf(), nullptr));

        ComPtr<ID3D12RootSignature> signature;
        THROW_IF_FAILED(_impl->device->CreateRootSignature(
            0,
            serialized_data->GetBufferPointer(), 
            serialized_data->GetBufferSize(), 
            __uuidof(ID3D12RootSignature), 
            reinterpret_cast<void**>(signature.GetAddressOf())
        ));

        _impl->signatures.emplace_back(signature);
        signature_index = static_cast<uint16_t>(_impl->signatures.size()) - 1;
    }

    auto& signature_data = _impl->signatures[signature_index];
    
    auto get_fill_mode = [&](FillMode const fill_mode) {
        switch(fill_mode) {
            case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
            case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
        }
        return D3D12_FILL_MODE_SOLID;
    };

    auto get_cull_mode = [&](CullMode const cull_mode) {
        switch(cull_mode) {
            case CullMode::Front: return D3D12_CULL_MODE_FRONT;
            case CullMode::Back: return D3D12_CULL_MODE_BACK;
            case CullMode::None: return D3D12_CULL_MODE_NONE;
        }
        return D3D12_CULL_MODE_NONE;
    };

    auto get_comparison_func = [&](CompareOp const compare_op) {
        switch(compare_op) {
            case CompareOp::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
            case CompareOp::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
            case CompareOp::Greater: return D3D12_COMPARISON_FUNC_GREATER;
            case CompareOp::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case CompareOp::Less: return D3D12_COMPARISON_FUNC_LESS;
            case CompareOp::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case CompareOp::Never: return D3D12_COMPARISON_FUNC_NEVER;
            case CompareOp::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        }
        return D3D12_COMPARISON_FUNC_ALWAYS;
    };

    auto get_blend_func = [&](BlendOp const blend_op) {
        switch(blend_op) {
            case BlendOp::Add: return D3D12_BLEND_OP_ADD;
            case BlendOp::Max: return D3D12_BLEND_OP_MAX;
            case BlendOp::Min: return D3D12_BLEND_OP_MIN;
            case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
            case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
        }
        return D3D12_BLEND_OP_ADD;
    };

    auto get_blend = [&](Blend const blend) {
        switch(blend) {
            case Blend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
            case Blend::One: return D3D12_BLEND_ONE;
            case Blend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
            case Blend::Zero: return D3D12_BLEND_ZERO; 
        }
        return D3D12_BLEND_ZERO;
    };

    auto get_dxgi_format = [&](Format const format) {
        switch(format) {
            case Format::RGB32: return DXGI_FORMAT_R32G32B32_FLOAT;
            case Format::RGBA32: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        return DXGI_FORMAT_R32G32B32_FLOAT;
    };

    auto pipeline_desc = D3D12_GRAPHICS_PIPELINE_STATE_DESC {};
    pipeline_desc.pRootSignature = signature_data.Get();

    std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements;
    input_elements.reserve(vertex_inputs.size());
    for(auto const& input : vertex_inputs) {
        auto element_desc = D3D12_INPUT_ELEMENT_DESC {};
        element_desc.SemanticName = input.semantic.c_str();
        element_desc.InputSlot = input.index;
        element_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element_desc.Format = get_dxgi_format(input.format);
        element_desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    }
    pipeline_desc.InputLayout.pInputElementDescs = input_elements.data();
    pipeline_desc.InputLayout.NumElements = static_cast<uint32_t>(input_elements.size());

    auto rasterizer_desc = D3D12_RASTERIZER_DESC {};
    rasterizer_desc.FillMode = get_fill_mode(rasterizer.fill_mode);
    rasterizer_desc.CullMode = get_cull_mode(rasterizer.cull_mode);
    rasterizer_desc.FrontCounterClockwise = false;
    rasterizer_desc.DepthBias = 0;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    rasterizer_desc.DepthClipEnable = true;
    rasterizer_desc.MultisampleEnable = false;
    rasterizer_desc.AntialiasedLineEnable = false;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    pipeline_desc.RasterizerState = rasterizer_desc;

    for(auto& shader_handle : shader_handles) {
        auto& shader_data = _impl->handles.shaders[shader_handle._id & 0xffff];

        auto bytecode = D3D12_SHADER_BYTECODE {};
        bytecode.pShaderBytecode = shader_data.data.data();
        bytecode.BytecodeLength = shader_data.data.size();
        
        switch(shader_data.visibility) {
            case D3D12_SHADER_VISIBILITY_VERTEX: pipeline_desc.VS = bytecode; break;
            case D3D12_SHADER_VISIBILITY_PIXEL: pipeline_desc.PS = bytecode; break;
            case D3D12_SHADER_VISIBILITY_GEOMETRY: pipeline_desc.GS = bytecode; break;
            case D3D12_SHADER_VISIBILITY_DOMAIN: pipeline_desc.DS = bytecode; break;
            case D3D12_SHADER_VISIBILITY_HULL: pipeline_desc.HS = bytecode; break;
        }
    }

    auto depth_stencil_desc = D3D12_DEPTH_STENCIL_DESC {};
    depth_stencil_desc.DepthFunc = get_comparison_func(depth_stencil.depth_func);
    depth_stencil_desc.DepthEnable = depth_stencil.write_enable;
    depth_stencil_desc.StencilEnable = depth_stencil.write_enable;
    depth_stencil_desc.DepthWriteMask = depth_stencil.write_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    depth_stencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    depth_stencil_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;

    auto depth_stencil_face = D3D12_DEPTH_STENCILOP_DESC {};
    depth_stencil_face.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_face.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_face.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_face.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    depth_stencil_desc.FrontFace = depth_stencil_face;
    depth_stencil_desc.BackFace = depth_stencil_face;

    pipeline_desc.DepthStencilState = depth_stencil_desc;

    auto blend_desc = D3D12_BLEND_DESC {};
    auto render_target_blend_desc = D3D12_RENDER_TARGET_BLEND_DESC {};
    render_target_blend_desc.BlendEnable = blend.blend_enable;
    render_target_blend_desc.SrcBlend = get_blend(blend.blend_src);
    render_target_blend_desc.DestBlend = get_blend(blend.blend_dst);
    render_target_blend_desc.BlendOp = get_blend_func(blend.blend_op);
    render_target_blend_desc.SrcBlendAlpha = get_blend(blend.blend_src_alpha);
    render_target_blend_desc.DestBlendAlpha = get_blend(blend.blend_dst_alpha);
    render_target_blend_desc.BlendOpAlpha = get_blend_func(blend.blend_op_alpha);
    render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    auto render_pass_data = _impl->handles.render_passes[render_pass_handle._id & 0xffff];

    for(size_t i = 0; i < render_pass_data.rtvs.size(); ++i) {
        blend_desc.RenderTarget[i] = render_target_blend_desc;
        pipeline_desc.RTVFormats[i] = render_pass_data.rtvs[i].BeginningAccess.Clear.ClearValue.Format;
    }
    pipeline_desc.DSVFormat = render_pass_data.dsv.has_value() ? render_pass_data.dsv.value().DepthBeginningAccess.Clear.ClearValue.Format : DXGI_FORMAT_UNKNOWN;
    pipeline_desc.NumRenderTargets = static_cast<uint32_t>(render_pass_data.rtvs.size());
    
    pipeline_desc.BlendState = blend_desc;

    pipeline_desc.SampleMask = std::numeric_limits<uint32_t>::max();

    // TODO
    pipeline_desc.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pipeline_state;
    THROW_IF_FAILED(_impl->device->CreateGraphicsPipelineState(
        &pipeline_desc, 
        __uuidof(ID3D12PipelineState), 
        reinterpret_cast<void**>(pipeline_state.GetAddressOf())
    ));

    size_t handle_id = _impl->handles.pipelines.push(
        Backend::Impl::PipelineData { 
            signature_index,
            pipeline_state
        }
    );
    return GPUResourceHandle(GPUResourceType::Pipeline, static_cast<uint16_t>(handle_id));
}

GPUResourceHandle Backend::create_command_buffer(QueueType const queue_type) {

    auto get_command_list_type = [&](QueueType const queue_type) -> D3D12_COMMAND_LIST_TYPE {
        switch(queue_type) {
            case QueueType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
            case QueueType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
            case QueueType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        }
        return D3D12_COMMAND_LIST_TYPE_DIRECT; 
    };

    ComPtr<ID3D12CommandAllocator> allocator;
    THROW_IF_FAILED(_impl->device->CreateCommandAllocator(
        get_command_list_type(queue_type), 
        __uuidof(ID3D12CommandAllocator), 
        reinterpret_cast<void**>(allocator.GetAddressOf())
    ));

    ComPtr<ID3D12GraphicsCommandList4> command_list;
    THROW_IF_FAILED(_impl->device->CreateCommandList1(
        0, 
        get_command_list_type(queue_type),
        D3D12_COMMAND_LIST_FLAG_NONE,
        __uuidof(ID3D12GraphicsCommandList4), 
        reinterpret_cast<void**>(command_list.GetAddressOf())
    ));

    return GPUResourceHandle(GPUResourceType::CommandBuffer, 
        static_cast<uint16_t>(_impl->handles.commands.push(
            Backend::Impl::CommandListData { 
                command_list,
                allocator
            }
        ))
    );
}

CommandGenerator Backend::create_command_generator(GPUResourceHandle const& handle) {

    return CommandGenerator(*this, handle._id & 0xffff);
}

void Backend::execute_command_buffers(QueueType const queue_type, std::vector<GPUResourceHandle> const& handles) {

    for(auto& handle : handles) {

        _impl->queues.direct->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(_impl->handles.commands[handle._id & 0xffff].first.GetAddressOf()));
    }
}

GPUResourceHandle Backend::get_swap_texture() const {

    return GPUResourceHandle(GPUResourceType::Texture, _impl->swap_indices[_impl->frame_index]);
}

void Backend::swap_buffers() {

    const uint64_t value = _impl->fence_values[_impl->frame_index];
    THROW_IF_FAILED(_impl->queues.direct->Signal(_impl->fences[_impl->frame_index].Get(), value));

    if(_impl->fences[_impl->frame_index]->GetCompletedValue() < value) {
        THROW_IF_FAILED(_impl->fences[_impl->frame_index]->SetEventOnCompletion(value, _impl->wait_event));
        WaitForSingleObjectEx(_impl->wait_event, INFINITE, false);
    }
    ++_impl->fence_values[_impl->frame_index];

    _impl->swapchain->Present(0, 0);

    _impl->frame_index = (_impl->frame_index + 1) % _impl->frame_count;
}

void Backend::resize_buffers(uint32_t const width, uint32_t const height, uint32_t const buffer_count) {

    for(uint32_t i = 0; i < _impl->frame_count; ++i) {
        const uint64_t value = _impl->fence_values[i];
        THROW_IF_FAILED(_impl->queues.direct->Signal(_impl->fences[i].Get(), value));

        if(_impl->fences[i]->GetCompletedValue() < value) {
            THROW_IF_FAILED(_impl->fences[i]->SetEventOnCompletion(value, _impl->wait_event));
            WaitForSingleObjectEx(_impl->wait_event, INFINITE, false);
        }
        ++_impl->fence_values[i];
    }
    
    auto swapchain_desc = DXGI_SWAP_CHAIN_DESC1 {};
    _impl->swapchain->GetDesc1(&swapchain_desc);
    _impl->swapchain->ResizeBuffers(buffer_count, width, height, swapchain_desc.Format, swapchain_desc.Flags);

    _impl->frame_index = (_impl->frame_index + 1) % _impl->frame_count;
}

CommandGenerator::CommandGenerator(Backend& backend, uint16_t const cmd_index) : _backend(&backend), _cmd_index(cmd_index) {

    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    THROW_IF_FAILED(command_data.first->Reset(command_data.second.Get(), nullptr));
}

CommandGenerator::~CommandGenerator() {

    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    command_data.first->Close();
}

CommandGenerator& CommandGenerator::set_viewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height) {

    auto viewport = D3D12_VIEWPORT {};
    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;

    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    command_data.first->RSSetViewports(1, &viewport);
    return *this;
}

CommandGenerator& CommandGenerator::set_scissor(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom) {

    auto rect = D3D12_RECT {};
    rect.top = static_cast<LONG>(top);
    rect.bottom = static_cast<LONG>(bottom);
    rect.left = static_cast<LONG>(left);
    rect.right = static_cast<LONG>(right);

    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    command_data.first->RSSetScissorRects(1, &rect);
    return *this;
}

CommandGenerator& CommandGenerator::bind_pipeline(GPUResourceHandle const& handle) {

    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    command_data.first->SetPipelineState(nullptr);
    return *this;
}

CommandGenerator& CommandGenerator::barrier(GPUResourceHandle const& handle, MemoryState const before, MemoryState const after) {

    auto resource_barrier = D3D12_RESOURCE_BARRIER {};
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    if(static_cast<uint16_t>((handle._id >> 16) & 0xffff) == static_cast<uint16_t>(GPUResourceType::Texture)) {
        resource_barrier.Transition.pResource = _backend->_impl->handles.textures[handle._id & 0xffff].resource.Get();  
    } else {
        // TO DO
    }

    auto get_memory_state = [&](MemoryState const state) -> D3D12_RESOURCE_STATES {
        switch(state) {
		    case MemoryState::Common: return D3D12_RESOURCE_STATE_COMMON;
		    case MemoryState::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		    case MemoryState::Present: return D3D12_RESOURCE_STATE_PRESENT;
		    // case MemoryState::GenericRead: return D3D12_RESOURCE_STATE_GENERIC_READ;
		    // case MemoryState::CopySource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		    // case MemoryState::CopyDest: return D3D12_RESOURCE_STATE_COPY_DEST;
        }
        return D3D12_RESOURCE_STATE_COMMON;
    };
    
    resource_barrier.Transition.StateBefore = get_memory_state(before);
    resource_barrier.Transition.StateAfter = get_memory_state(after);

    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    command_data.first->ResourceBarrier(1, &resource_barrier);
    return *this;
}
    
CommandGenerator& CommandGenerator::begin_render_pass(
    uint16_t const rtv_count,
    std::array<GPUResourceHandle, 8> const& rtv_handles,
    std::array<RenderPassColorDesc, 8> const& rtv_ops,
    std::array<Color, 8> const& rtv_clears,
    GPUResourceHandle dsv_handle,
    RenderPassDepthStencilDesc dsv_op,
    std::pair<float, uint8_t> dsv_clear
) {

    auto get_render_pass_begin_type = [&](RenderPassLoadOp const load_op) {
        switch (load_op) {
    	    case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	    case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		    case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    };
    
    auto get_render_pass_end_type = [&](RenderPassStoreOp const store_op) {
        switch (store_op) {
    	    case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	    case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    };

    std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, 8> render_pass_rtvs; 

    for(uint16_t i = 0; i < rtv_count; ++i) {
        auto& texture_data = _backend->_impl->handles.textures[rtv_handles[i]._id & 0xffff];

        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            texture_data.descriptor_alloc.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _backend->_impl->descriptor_sizes.rtv * // Device descriptor size
            texture_data.descriptor_alloc.offset // Allocation offset
        };

        auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        begin.Type = get_render_pass_begin_type(rtv_ops[i].load_op);

        begin.Clear.ClearValue.Format = texture_data.resource->GetDesc().Format;
        std::memcpy(begin.Clear.ClearValue.Color, &rtv_clears[i], sizeof(Color));

        auto end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        end.Type = get_render_pass_end_type(rtv_ops[i].store_op);

        render_pass_rtvs[i].BeginningAccess = begin;
        render_pass_rtvs[i].EndingAccess = end;
        render_pass_rtvs[i].cpuDescriptor = cpu_handle;
    }

    auto render_pass_dsv = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC {};
    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    if(dsv_handle != GPUResourceHandle()) {
        // TO DO
    } else {
        command_data.first->BeginRenderPass(static_cast<uint32_t>(rtv_count), render_pass_rtvs.data(), nullptr, D3D12_RENDER_PASS_FLAG_NONE);
    }

    command_data.first->EndRenderPass();
    return *this;
}

CommandGenerator& CommandGenerator::end_render_pass() {

    auto& command_data = _backend->_impl->handles.commands[_cmd_index];
    command_data.first->EndRenderPass();
    return *this;
}