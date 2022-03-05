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
using ionengine::Handle;
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
    size_t _heap_size;
};

MemoryPool::MemoryPool(ID3D12Device4* device, D3D12_HEAP_TYPE const heap_type, size_t const heap_size) : _heap_size(heap_size) {

    D3D12_HEAP_DESC heap_desc{};
    heap_desc.SizeInBytes = heap_size;
    heap_desc.Properties.Type = heap_type;

    ComPtr<ID3D12Heap> heap;
    THROW_IF_FAILED(device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(heap.GetAddressOf())));

    _heaps.emplace_back(MemoryHeap { heap, std::vector<uint8_t>(heap_size / 1048576, 0x0), 0 });
}

MemoryAllocInfo MemoryPool::allocate(size_t const size) {

    auto get_align_size = [&](size_t const size) -> size_t {
        return size < 1048576 ? 1048576 : (size % 1048576) > 0 ? (size / 1048576 + 1) * 1048576 : size;
    };

    auto memory_alloc_info = MemoryAllocInfo {};

    size_t align_size = get_align_size(size);

    for(auto& heap : _heaps) {
        if(heap.offset + align_size > _heap_size) {
            continue;
        } else {
            size_t alloc_size = 0;
            for(uint64_t i = 0; i < static_cast<uint64_t>(heap.blocks.size()); ++i) {
                if(alloc_size == align_size) {
                    std::memset(heap.blocks.data() + memory_alloc_info.offset / 1048576, 0x1, sizeof(uint8_t) * align_size / 1048576);
                    break;
                }

                if(alloc_size == 0) {
                    memory_alloc_info.heap = &heap;
                    memory_alloc_info.offset = i * 1048576;
                    memory_alloc_info.size = align_size;
                }

                alloc_size = heap.blocks[i] == 0x0 ? alloc_size + 1048576 : 0;
            }
            if(alloc_size != align_size) {
                memory_alloc_info.heap = nullptr;
                memory_alloc_info.offset = 0;
                memory_alloc_info.size = 0;
            }
        }
        if(memory_alloc_info.heap) {
            break;
        }
    }

    if(!memory_alloc_info.heap) {

    }

    return memory_alloc_info;
}

void MemoryPool::deallocate(MemoryAllocInfo const& alloc_info) {

    std::memset(alloc_info.heap->blocks.data() + alloc_info.offset / 1048576, 0x0, sizeof(uint8_t) * alloc_info.size / 1048576);
    --alloc_info.heap->offset;
}

class DescriptorPool {
public:

    DescriptorPool() = default;

    DescriptorPool(DescriptorPool const&) = delete;

    DescriptorPool(DescriptorPool&&) noexcept = default;

    DescriptorPool& operator=(DescriptorPool const&) = delete;

    DescriptorPool& operator=(DescriptorPool&&) = default;

    DescriptorPool(ID3D12Device4* device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const heap_size, bool const shader_visible);

    DescriptorAllocInfo allocate();
    
    void deallocate(DescriptorAllocInfo const& alloc_info);

private:

    std::list<DescriptorHeap> _heaps;
    uint32_t _heap_size;
};

DescriptorPool::DescriptorPool(ID3D12Device4* device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const heap_size, bool const shader_visible) : _heap_size(heap_size) {

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
    heap_desc.NumDescriptors = heap_size;
    heap_desc.Type = heap_type;
    heap_desc.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    ComPtr<ID3D12DescriptorHeap> heap;
    THROW_IF_FAILED(device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(heap.GetAddressOf())));

    _heaps.emplace_back(DescriptorHeap { heap, std::vector<uint8_t>(heap_size, 0x0), 0 });
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

namespace ionengine::renderer {

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

struct Sampler {
    DescriptorAllocInfo descriptor_alloc_info;
};

struct DescriptorLayout {
    ComPtr<ID3D12RootSignature> root_signature;
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptor_ranges;
};

struct DescriptorSet {
    ComPtr<ID3D12RootSignature> root_signature;
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptor_ranges;
    std::vector<DescriptorAllocInfo> descriptor_alloc_infos;
};

struct Pipeline {
    ComPtr<ID3D12PipelineState> pipeline_state;
    std::vector<uint32_t> vertex_strides;
};

struct Shader {
    std::vector<char8_t> bytecode;
    D3D12_SHADER_VISIBILITY type;
};

struct RenderPass {
    std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> colors;
    std::optional<D3D12_RENDER_PASS_DEPTH_STENCIL_DESC> depth_stencil;
};

}

struct Backend::Impl {

    ComPtr<IDXGIFactory4> factory;
    ComPtr<ID3D12Debug> debug;
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device4> device;
    ComPtr<IDXGISwapChain3> swapchain;

    ComPtr<ID3D12CommandQueue> direct_queue;
    ComPtr<ID3D12CommandQueue> copy_queue;
    ComPtr<ID3D12CommandQueue> compute_queue;

    HANDLE wait_event;

    struct CommandBuffer {
        ComPtr<ID3D12CommandAllocator> command_allocator;
        ComPtr<ID3D12GraphicsCommandList4> command_list;
        Handle<Pipeline> current_pipeline;
    };

    struct Fence {
        ComPtr<ID3D12Fence> fence;
        uint64_t fence_value;
    };

    struct StageBuffer {
        ComPtr<ID3D12Resource> resource;
        MemoryAllocInfo memory_alloc_info;
        uint64_t offset;
    };

    struct Frame {
        Fence direct_fence;
        Fence copy_fence;
        Fence compute_fence;
        
        CommandBuffer direct_buffer;
        CommandBuffer copy_buffer;
        CommandBuffer compute_buffer;
        
        Handle<Texture> texture;

        StageBuffer stage_buffer;
    };

    std::vector<Frame> frames;
    uint16_t frame_index;

    MemoryPool local_memory;
    MemoryPool host_visible_memory;

    DescriptorPool rtv_descriptor;
    DescriptorPool sampler_descriptor;
    DescriptorPool srv_descriptor;
    DescriptorPool dsv_descriptor;
    DescriptorPool srv_shader_pool;
    DescriptorPool sampler_shader_pool;

    DescriptorAllocInfo null_srv_descriptor_alloc_info;
    DescriptorAllocInfo null_sampler_descriptor_alloc_info;

    InstanceContainer<Texture> textures;
    InstanceContainer<Buffer> buffers;
    InstanceContainer<Sampler> samplers;
    InstanceContainer<Pipeline> pipelines;
    InstanceContainer<Shader> shaders;
    InstanceContainer<RenderPass> render_passes;
    InstanceContainer<DescriptorLayout> descriptor_layouts;
    InstanceContainer<DescriptorSet> descriptor_sets;

    CommandBuffer* current_buffer;
};

Backend::Backend(uint32_t const adapter_index, platform::Window* const window, uint32_t const frame_count) : _impl(std::make_unique<Impl>()) {

    THROW_IF_FAILED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(_impl->debug.GetAddressOf())));
    _impl->debug->EnableDebugLayer();

    THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(_impl->factory.GetAddressOf())));

    THROW_IF_FAILED(_impl->factory->EnumAdapters1(adapter_index, _impl->adapter.GetAddressOf()));

    THROW_IF_FAILED(D3D12CreateDevice(_impl->adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4), reinterpret_cast<void**>(_impl->device.GetAddressOf())));

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    THROW_IF_FAILED(_impl->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(_impl->direct_queue.GetAddressOf())));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    THROW_IF_FAILED(_impl->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(_impl->copy_queue.GetAddressOf())));

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    THROW_IF_FAILED(_impl->device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(_impl->compute_queue.GetAddressOf())));

    _impl->rtv_descriptor = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 64, false);
    _impl->dsv_descriptor = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 16, false);
    _impl->sampler_descriptor = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 64, false);
    _impl->srv_descriptor = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, false);
    
    _impl->local_memory = MemoryPool(_impl->device.Get(), D3D12_HEAP_TYPE_DEFAULT, 1024 * 1024 * 256);
    _impl->host_visible_memory = MemoryPool(_impl->device.Get(), D3D12_HEAP_TYPE_UPLOAD, 1024 * 1024 * 128);

    _impl->srv_shader_pool = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, true);
    _impl->sampler_shader_pool = DescriptorPool(_impl->device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024, true);

    _impl->null_srv_descriptor_alloc_info = _impl->srv_shader_pool.allocate();
    _impl->null_sampler_descriptor_alloc_info = _impl->sampler_shader_pool.allocate();
    
    platform::Size window_size = window->get_size();

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
        _impl->direct_queue.Get(), 
        reinterpret_cast<HWND>(window->get_handle()), 
        &swapchain_desc, 
        nullptr, 
        nullptr, 
        reinterpret_cast<IDXGISwapChain1**>(_impl->swapchain.GetAddressOf()))
    );

    _impl->frames.reserve(frame_count);

    for(uint32_t i = 0; i < frame_count; ++i) {
        ComPtr<ID3D12Resource> resource;
        THROW_IF_FAILED(_impl->swapchain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource.GetAddressOf())));

        auto view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
        view_desc.Format = swapchain_desc.Format;
        view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        DescriptorAllocInfo descriptor_alloc_info = _impl->rtv_descriptor.allocate();

        const uint32_t rtv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            rtv_size * // Device descriptor size
            descriptor_alloc_info.offset // Allocation offset
        };
        _impl->device->CreateRenderTargetView(resource.Get(), &view_desc, cpu_handle);

        auto frame = Backend::Impl::Frame {}; 
        frame.texture = _impl->textures.push(Texture { resource, {}, descriptor_alloc_info });

        THROW_IF_FAILED(_impl->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(frame.direct_fence.fence.GetAddressOf())));
        ++frame.direct_fence.fence_value;

        THROW_IF_FAILED(_impl->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(frame.copy_fence.fence.GetAddressOf())));
        ++frame.copy_fence.fence_value;

        THROW_IF_FAILED(_impl->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(frame.compute_fence.fence.GetAddressOf())));
        ++frame.compute_fence.fence_value;

        THROW_IF_FAILED(_impl->device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            __uuidof(ID3D12CommandAllocator), 
            reinterpret_cast<void**>(frame.direct_buffer.command_allocator.GetAddressOf()))
        );
        
        THROW_IF_FAILED(_impl->device->CreateCommandList1(
            0, 
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            D3D12_COMMAND_LIST_FLAG_NONE,
            __uuidof(ID3D12GraphicsCommandList4),
            reinterpret_cast<void**>(frame.direct_buffer.command_list.GetAddressOf()))
        );

        THROW_IF_FAILED(_impl->device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_COPY, 
            __uuidof(ID3D12CommandAllocator), 
            reinterpret_cast<void**>(frame.copy_buffer.command_allocator.GetAddressOf()))
        );
        
        THROW_IF_FAILED(_impl->device->CreateCommandList1(
            0, 
            D3D12_COMMAND_LIST_TYPE_COPY, 
            D3D12_COMMAND_LIST_FLAG_NONE,
            __uuidof(ID3D12GraphicsCommandList4),
            reinterpret_cast<void**>(frame.copy_buffer.command_list.GetAddressOf()))
        );

        THROW_IF_FAILED(_impl->device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_COMPUTE, 
            __uuidof(ID3D12CommandAllocator), 
            reinterpret_cast<void**>(frame.compute_buffer.command_allocator.GetAddressOf()))
        );
        
        THROW_IF_FAILED(_impl->device->CreateCommandList1(
            0, 
            D3D12_COMMAND_LIST_TYPE_COMPUTE, 
            D3D12_COMMAND_LIST_FLAG_NONE,
            __uuidof(ID3D12GraphicsCommandList4),
            reinterpret_cast<void**>(frame.compute_buffer.command_list.GetAddressOf()))
        );

        auto resource_desc = D3D12_RESOURCE_DESC {};
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resource_desc.Width = 1024 * 1024 * 32;
        resource_desc.Height = 1;
        resource_desc.MipLevels = 1;
        resource_desc.DepthOrArraySize = 1;
        resource_desc.SampleDesc.Count = 1;
        resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        frame.stage_buffer.memory_alloc_info = _impl->host_visible_memory.allocate(resource_desc.Width);

        THROW_IF_FAILED(_impl->device->CreatePlacedResource(
            frame.stage_buffer.memory_alloc_info.heap->heap.Get(),
            frame.stage_buffer.memory_alloc_info.offset,
            &resource_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            __uuidof(ID3D12Resource), 
            reinterpret_cast<void**>(frame.stage_buffer.resource.GetAddressOf())
        ));

        _impl->frames.emplace_back(frame);
    }

    _impl->wait_event = CreateEvent(nullptr, false, false, nullptr);
    if(!_impl->wait_event) {
        THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
    }
}

Backend::~Backend() = default;

Handle<Texture> Backend::create_texture(
    Dimension const dimension,
    Extent2D const extent,
    uint16_t const mip_levels,
    uint16_t const array_layers,
    Format const format,
    BackendFlags const flags
) {
    auto resource_desc = D3D12_RESOURCE_DESC {};
    switch(dimension) {
        case Dimension::_1D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D; break;
        case Dimension::_2D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; break;
        case Dimension::_3D: resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; break;
    }
    resource_desc.Width = extent.width;
    resource_desc.Height = extent.height;
    resource_desc.MipLevels = mip_levels;
    resource_desc.DepthOrArraySize = array_layers;
    resource_desc.SampleDesc.Count = 1;
    switch(format) {
        case Format::Unknown: resource_desc.Format = DXGI_FORMAT_UNKNOWN; break;
        case Format::RGBA8Unorm: resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
        case Format::BGRA8: resource_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; break;
        case Format::BGR8: resource_desc.Format = DXGI_FORMAT_B8G8R8X8_UNORM; break;
        case Format::BC1: resource_desc.Format = DXGI_FORMAT_BC1_UNORM; break;
        case Format::BC5: resource_desc.Format = DXGI_FORMAT_BC5_UNORM; break;
    }
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_RESOURCE_ALLOCATION_INFO res_alloc_info = _impl->device->GetResourceAllocationInfo(0, 1, &resource_desc);
    MemoryAllocInfo mem_alloc_info = _impl->local_memory.allocate(res_alloc_info.SizeInBytes + res_alloc_info.Alignment);

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
        case BackendFlags::RenderTarget: {
            auto view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
            view_desc.Format = resource_desc.Format;
            switch(dimension) {
                case Dimension::_1D: {
                    view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                    view_desc.Texture1D.MipSlice = mip_levels;
                } break;
                case Dimension::_2D: {
                    view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                    view_desc.Texture2D.MipSlice = mip_levels;
                    view_desc.Texture2D.PlaneSlice = array_layers;
                } break;
                case Dimension::_3D: {
                    view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                    view_desc.Texture3D.MipSlice = mip_levels;
                    view_desc.Texture3D.WSize = array_layers;
                } break;
            }

            descriptor_alloc_info = _impl->rtv_descriptor.allocate();

            const uint32_t rtv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                rtv_size * // Device descriptor size
                descriptor_alloc_info.offset // Allocation offset
            };
            _impl->device->CreateRenderTargetView(resource.Get(), &view_desc, cpu_handle);
        } break;
        
        case BackendFlags::DepthStencil: {
            auto view_desc = D3D12_DEPTH_STENCIL_VIEW_DESC {};
            view_desc.Format = resource_desc.Format;
            switch(dimension) {
                case Dimension::_1D: {
                    view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
                    view_desc.Texture1D.MipSlice = mip_levels;
                } break;
                case Dimension::_2D: {
                    view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    view_desc.Texture2D.MipSlice = mip_levels;
                } break;
                default: {
                    assert(false && "Depth stencil view dimension is unsupported");
                } break;
            }

            descriptor_alloc_info = _impl->dsv_descriptor.allocate();

            const uint32_t dsv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
            auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                dsv_size * // Device descriptor size
                descriptor_alloc_info.offset // Allocation offset
            };
            _impl->device->CreateDepthStencilView(resource.Get(), &view_desc, cpu_handle);
        } break;

        case BackendFlags::UnorderedAccess: {
            // TO DO
        } break;

        default: {
            auto view_desc = D3D12_SHADER_RESOURCE_VIEW_DESC {};
            view_desc.Format = resource_desc.Format;
            switch(dimension) {
                case Dimension::_1D: {
                    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                    view_desc.Texture1D.MipLevels = mip_levels;
                } break;
                case Dimension::_2D: {
                    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    view_desc.Texture2D.MipLevels = mip_levels;
                } break;
                case Dimension::_3D: {
                    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                    view_desc.Texture3D.MipLevels = mip_levels;
                } break;
            }
            view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            descriptor_alloc_info = _impl->srv_descriptor.allocate();

            const uint32_t srv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                srv_size * // Device descriptor size
                descriptor_alloc_info.offset // Allocation offset
            };
            _impl->device->CreateShaderResourceView(resource.Get(), &view_desc, cpu_handle);
        } break;
    }

    return _impl->textures.push(Texture { resource, mem_alloc_info, descriptor_alloc_info });
}

Handle<Buffer> Backend::create_buffer(size_t const size, BackendFlags const flags) {

    auto resource_desc = D3D12_RESOURCE_DESC {};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = size;
    resource_desc.Height = 1;
    resource_desc.MipLevels = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_RESOURCE_ALLOCATION_INFO res_alloc_info = _impl->device->GetResourceAllocationInfo(0, 1, &resource_desc);
    MemoryAllocInfo mem_alloc_info;
    D3D12_RESOURCE_STATES initial_state;

    if(flags & BackendFlags::HostVisible) {
        mem_alloc_info = _impl->host_visible_memory.allocate(res_alloc_info.SizeInBytes + res_alloc_info.Alignment);
        initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else {
        mem_alloc_info = _impl->local_memory.allocate(res_alloc_info.SizeInBytes + res_alloc_info.Alignment);
        initial_state = D3D12_RESOURCE_STATE_COMMON;
    }

    ComPtr<ID3D12Resource> resource;
    THROW_IF_FAILED(_impl->device->CreatePlacedResource(
        mem_alloc_info.heap->heap.Get(),
        mem_alloc_info.offset,
        &resource_desc,
        initial_state,
        nullptr,
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(resource.GetAddressOf())
    ));

    auto descriptor_alloc_info = DescriptorAllocInfo {};
    
    if(flags & BackendFlags::ConstantBuffer) {
        auto view_desc = D3D12_CONSTANT_BUFFER_VIEW_DESC {};
        view_desc.BufferLocation = resource->GetGPUVirtualAddress();
        view_desc.SizeInBytes = static_cast<uint32_t>(size);

        descriptor_alloc_info = _impl->srv_descriptor.allocate();

        const uint32_t srv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
            descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            srv_size * // Device descriptor size
            descriptor_alloc_info.offset // Allocation offset
        };
        _impl->device->CreateConstantBufferView(&view_desc, cpu_handle);
    }

    return _impl->buffers.push(Buffer {
        resource,
        mem_alloc_info,
        descriptor_alloc_info
    });
}

Handle<RenderPass> Backend::create_render_pass(
    std::vector<Handle<Texture>> const& rtv_handles,
    std::vector<RenderPassColorDesc> const& rtv_ops,
    Handle<Texture> const& dsv_handle,
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
        auto& texture = _impl->textures.get(rtv_handles[i]);

        const uint32_t rtv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            texture.descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            rtv_size * // Device descriptor size
            texture.descriptor_alloc_info.offset // Allocation offset
        };

        auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        begin.Type = get_render_pass_begin_type(rtv_ops[i].load_op);
        begin.Clear.ClearValue.Format = texture.resource->GetDesc().Format;

        auto end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        end.Type = get_render_pass_end_type(rtv_ops[i].store_op);

        rtvs[i].BeginningAccess = begin;
        rtvs[i].EndingAccess = end;
        rtvs[i].cpuDescriptor = cpu_handle;
    }

    std::optional<D3D12_RENDER_PASS_DEPTH_STENCIL_DESC> dsv_opt;

    if(dsv_handle != Handle<Texture>()) {
        auto& texture = _impl->textures.get(dsv_handle);

        const uint32_t dsv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            texture.descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            dsv_size * // Device descriptor size
            texture.descriptor_alloc_info.offset // Allocation offset
        };

        auto depth_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        depth_begin.Type = get_render_pass_begin_type(dsv_op.depth_load_op);
        depth_begin.Clear.ClearValue.Format = texture.resource->GetDesc().Format;

        auto stencil_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        stencil_begin.Type = get_render_pass_begin_type(dsv_op.stencil_load_op);
        stencil_begin.Clear.ClearValue.Format = texture.resource->GetDesc().Format;

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

    return _impl->render_passes.push(RenderPass { rtvs, dsv_opt });
}

Handle<Sampler> Backend::create_sampler(
    Filter const filter,
    AddressMode const address_u,
    AddressMode const address_v,
    AddressMode const address_w,
    uint16_t const aniso,
    CompareOp const compare_op
) {

    auto get_filter = [&](Filter const filter) -> D3D12_FILTER {
        switch(filter) {
            case Filter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
            case Filter::MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            case Filter::ComparisonMinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        }
        return D3D12_FILTER_ANISOTROPIC;
    };

    auto get_address_mode = [&](AddressMode const address_mode) -> D3D12_TEXTURE_ADDRESS_MODE {
        switch(address_mode) {
            case AddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            case AddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        }
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
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

    auto sampler_desc = D3D12_SAMPLER_DESC {};
    sampler_desc.Filter = get_filter(filter);
    sampler_desc.AddressU = get_address_mode(address_u);
    sampler_desc.AddressV = get_address_mode(address_v);
    sampler_desc.AddressW = get_address_mode(address_w);
    sampler_desc.ComparisonFunc = get_comparison_func(compare_op);
    sampler_desc.MaxAnisotropy = aniso;
    sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
    
    DescriptorAllocInfo descriptor_alloc_info = _impl->sampler_descriptor.allocate();

    const uint32_t sampler_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    auto cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
        descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
        sampler_size * // Device descriptor size
        descriptor_alloc_info.offset // Allocation offset
    };

    _impl->device->CreateSampler(&sampler_desc, cpu_handle);

    return _impl->samplers.push(Sampler { descriptor_alloc_info });
}

Handle<Shader> Backend::create_shader(std::span<char8_t> const shader_data, BackendFlags const flags) {

    std::vector<char8_t> data(shader_data.size());
    std::copy(shader_data.begin(), shader_data.end(), data.begin());
    
    D3D12_SHADER_VISIBILITY type;
    switch(flags) {
        case BackendFlags::VertexShader: type = D3D12_SHADER_VISIBILITY_VERTEX; break;
        case BackendFlags::PixelShader: type = D3D12_SHADER_VISIBILITY_PIXEL; break;
        case BackendFlags::GeometryShader: type = D3D12_SHADER_VISIBILITY_GEOMETRY; break;
        case BackendFlags::DomainShader: type = D3D12_SHADER_VISIBILITY_DOMAIN; break;
        case BackendFlags::HullShader: type = D3D12_SHADER_VISIBILITY_HULL; break;
        default: assert(false && "Shader visibility is unsupported"); break;
    }

    return _impl->shaders.push(Shader { data, type });
}

Handle<DescriptorLayout> Backend::create_descriptor_layout(std::span<DescriptorRangeDesc> const ranges) {

    std::vector<D3D12_DESCRIPTOR_RANGE> _ranges;
    _ranges.reserve(ranges.size());

    std::vector<D3D12_ROOT_PARAMETER> parameters;
    parameters.reserve(ranges.size());

    auto get_descriptor_range_type = [&](DescriptorRangeType const range_type) -> D3D12_DESCRIPTOR_RANGE_TYPE {
        switch(range_type) {
            case DescriptorRangeType::ShaderResource: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            case DescriptorRangeType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            case DescriptorRangeType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            case DescriptorRangeType::UnorderedAccess: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        }
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    };

    auto get_shader_visibility = [&](BackendFlags const flags) -> D3D12_SHADER_VISIBILITY {
        switch(flags) {
            case BackendFlags::VertexShader: return D3D12_SHADER_VISIBILITY_VERTEX;
            case BackendFlags::PixelShader: return D3D12_SHADER_VISIBILITY_PIXEL;
            case BackendFlags::GeometryShader: return D3D12_SHADER_VISIBILITY_GEOMETRY;
            case BackendFlags::DomainShader: return D3D12_SHADER_VISIBILITY_DOMAIN;
            case BackendFlags::HullShader: return D3D12_SHADER_VISIBILITY_HULL;
            case BackendFlags::AllShader: return D3D12_SHADER_VISIBILITY_ALL;
        }
        return D3D12_SHADER_VISIBILITY_ALL;
    };

    std::map<D3D12_DESCRIPTOR_RANGE_TYPE, uint32_t> index_ranges;
    index_ranges[D3D12_DESCRIPTOR_RANGE_TYPE_CBV] = 0;
    index_ranges[D3D12_DESCRIPTOR_RANGE_TYPE_SRV] = 0;
    index_ranges[D3D12_DESCRIPTOR_RANGE_TYPE_UAV] = 0;
    index_ranges[D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER] = 0;

    for(auto& range : ranges) {
        auto _range = D3D12_DESCRIPTOR_RANGE {};
        _range.RangeType = get_descriptor_range_type(range.range_type);
        _range.NumDescriptors = range.count;
        _range.BaseShaderRegister = index_ranges[_range.RangeType];
        _range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        index_ranges[_range.RangeType] += _range.NumDescriptors;
        _ranges.emplace_back(_range);

        auto parameter = D3D12_ROOT_PARAMETER {};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable.pDescriptorRanges = &_ranges.back();
        parameter.DescriptorTable.NumDescriptorRanges = 1;
        parameter.ShaderVisibility = get_shader_visibility(range.flags);

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

    return _impl->descriptor_layouts.push(DescriptorLayout { signature, _ranges });
}

Handle<Pipeline> Backend::create_pipeline(
    Handle<DescriptorLayout> const& layout_handle,
    std::span<VertexInputDesc> const vertex_inputs,
    std::span<Handle<Shader>> const shader_handles,
    RasterizerDesc const& rasterizer,
    DepthStencilDesc const& depth_stencil,
    BlendDesc const& blend,
    Handle<RenderPass> const& render_pass_handle
) {
    
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
            case Format::RG32: return DXGI_FORMAT_R32G32_FLOAT;
        }
        return DXGI_FORMAT_R32G32B32_FLOAT;
    };

    auto& signature = _impl->descriptor_layouts.get(layout_handle);

    auto pipeline_desc = D3D12_GRAPHICS_PIPELINE_STATE_DESC {};
    pipeline_desc.pRootSignature = signature.root_signature.Get();

    std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements;
    input_elements.reserve(vertex_inputs.size());

    std::vector<uint32_t> vertex_strides;
    vertex_strides.resize(15);

    for(auto const& input : vertex_inputs) {
        auto element_desc = D3D12_INPUT_ELEMENT_DESC {};
        element_desc.SemanticName = input.semantic.c_str();
        element_desc.InputSlot = input.slot;
        element_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element_desc.Format = get_dxgi_format(input.format);
        element_desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        input_elements.emplace_back(element_desc);

        vertex_strides[input.slot] += input.stride;
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
        auto& shader = _impl->shaders.get(shader_handle);

        auto bytecode = D3D12_SHADER_BYTECODE {};
        bytecode.pShaderBytecode = shader.bytecode.data();
        bytecode.BytecodeLength = shader.bytecode.size();
        
        switch(shader.type) {
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

    auto render_pass = _impl->render_passes.get(render_pass_handle);

    for(size_t i = 0; i < render_pass.colors.size(); ++i) {
        blend_desc.RenderTarget[i] = render_target_blend_desc;
        pipeline_desc.RTVFormats[i] = render_pass.colors[i].BeginningAccess.Clear.ClearValue.Format;
    }
    pipeline_desc.DSVFormat = render_pass.depth_stencil.has_value() ? render_pass.depth_stencil.value().DepthBeginningAccess.Clear.ClearValue.Format : DXGI_FORMAT_UNKNOWN;
    pipeline_desc.NumRenderTargets = static_cast<uint32_t>(render_pass.colors.size());
    
    pipeline_desc.BlendState = blend_desc;

    pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    pipeline_desc.SampleMask = std::numeric_limits<uint32_t>::max();

    // TODO
    pipeline_desc.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pipeline_state;
    THROW_IF_FAILED(_impl->device->CreateGraphicsPipelineState(
        &pipeline_desc, 
        __uuidof(ID3D12PipelineState), 
        reinterpret_cast<void**>(pipeline_state.GetAddressOf())
    ));

    return _impl->pipelines.push(Pipeline { pipeline_state, vertex_strides });
}

Handle<DescriptorSet> Backend::create_descriptor_set(Handle<DescriptorLayout> const& handle) {

    auto& descriptor_layout = _impl->descriptor_layouts.get(handle);

    std::vector<DescriptorAllocInfo> descriptor_alloc_infos;
    
    for(auto& range : descriptor_layout.descriptor_ranges) {
        
        for(uint32_t i = 0; i < range.NumDescriptors; ++i) {
            
            DescriptorAllocInfo descriptor_alloc_info;
            switch(range.RangeType) {
                case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                case D3D12_DESCRIPTOR_RANGE_TYPE_UAV: descriptor_alloc_info = _impl->srv_shader_pool.allocate(); break;
                case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: descriptor_alloc_info = _impl->sampler_shader_pool.allocate(); break;
            }
            descriptor_alloc_infos.emplace_back(descriptor_alloc_info);
        }
    }
    
    return _impl->descriptor_sets.push(DescriptorSet { descriptor_layout.root_signature, descriptor_layout.descriptor_ranges, descriptor_alloc_infos });
}

void Backend::write_descriptor_set(Handle<DescriptorSet> const& handle, std::span<DescriptorWriteDesc> const writes) {

    auto& descriptor_set = _impl->descriptor_sets.get(handle);

    for(auto& write : writes) {

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Handle<Texture>>) {
                auto& texture = _impl->textures.get(arg);
                const uint32_t srv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                auto src_cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                    texture.descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    srv_size * // Device descriptor size
                    texture.descriptor_alloc_info.offset // Allocation offset
                };
                auto dst_cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                    descriptor_set.descriptor_alloc_infos[write.index].heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    srv_size * // Device descriptor size
                    descriptor_set.descriptor_alloc_infos[write.index].offset // Allocation offset
                };
                _impl->device->CopyDescriptorsSimple(1, dst_cpu_handle, src_cpu_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            } else if constexpr (std::is_same_v<T, Handle<Buffer>>) {
                auto& buffer = _impl->buffers.get(arg);
                const uint32_t srv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                auto src_cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                    buffer.descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    srv_size * // Device descriptor size
                    buffer.descriptor_alloc_info.offset // Allocation offset
                };
                auto dst_cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                    descriptor_set.descriptor_alloc_infos[write.index].heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    srv_size * // Device descriptor size
                    descriptor_set.descriptor_alloc_infos[write.index].offset // Allocation offset
                };
                _impl->device->CopyDescriptorsSimple(1, dst_cpu_handle, src_cpu_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
            else if constexpr (std::is_same_v<T, Handle<Sampler>>) {
                auto& sampler = _impl->samplers.get(arg);
                const uint32_t sampler_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                auto src_cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                    sampler.descriptor_alloc_info.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    sampler_size * // Device descriptor size
                    sampler.descriptor_alloc_info.offset // Allocation offset
                };
                auto dst_cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                    descriptor_set.descriptor_alloc_infos[write.index].heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    sampler_size * // Device descriptor size
                    descriptor_set.descriptor_alloc_infos[write.index].offset // Allocation offset
                };
                _impl->device->CopyDescriptorsSimple(1, dst_cpu_handle, src_cpu_handle, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
            } else {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        }, write.data); 
    }
}

void Backend::bind_descriptor_set(Handle<DescriptorSet> const& handle) {

    auto& descriptor_set = _impl->descriptor_sets.get(handle);

    _impl->current_buffer->command_list->SetGraphicsRootSignature(descriptor_set.root_signature.Get());

    size_t binding = 0;
    for(uint32_t i = 0; i < descriptor_set.descriptor_ranges.size(); ++i) {

        D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
        switch(descriptor_set.descriptor_ranges[i].RangeType) {
            case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
            case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
            case D3D12_DESCRIPTOR_RANGE_TYPE_UAV: {
                const uint32_t srv_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                gpu_handle = D3D12_GPU_DESCRIPTOR_HANDLE {
                    descriptor_set.descriptor_alloc_infos[binding].heap->heap->GetGPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    srv_size * // Device descriptor size
                    descriptor_set.descriptor_alloc_infos[binding].offset // Allocation offset
                };
            } break;
            case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: {
                const uint32_t sampler_size = _impl->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                gpu_handle = D3D12_GPU_DESCRIPTOR_HANDLE {
                    descriptor_set.descriptor_alloc_infos[binding].heap->heap->GetGPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
                    sampler_size * // Device descriptor size
                    descriptor_set.descriptor_alloc_infos[binding].offset // Allocation offset
                };
            } break;
        }

        binding += descriptor_set.descriptor_ranges[i].NumDescriptors;
        _impl->current_buffer->command_list->SetGraphicsRootDescriptorTable(i, gpu_handle);     
    }
}

Handle<Texture> Backend::get_current_buffer() const {

    return _impl->frames[_impl->frame_index].texture;
}

void Backend::swap_buffers() {

    const uint64_t value = _impl->frames[_impl->frame_index].direct_fence.fence_value;
    THROW_IF_FAILED(_impl->direct_queue->Signal(_impl->frames[_impl->frame_index].direct_fence.fence.Get(), value));

    if(_impl->frames[_impl->frame_index].direct_fence.fence->GetCompletedValue() < value) {
        THROW_IF_FAILED(_impl->frames[_impl->frame_index].direct_fence.fence->SetEventOnCompletion(value, _impl->wait_event));
        WaitForSingleObjectEx(_impl->wait_event, INFINITE, false);
    }
    ++_impl->frames[_impl->frame_index].direct_fence.fence_value;

    _impl->swapchain->Present(0, 0);

    _impl->frame_index = (_impl->frame_index + 1) % _impl->frames.size();
}

void Backend::set_viewport(uint32_t const x, uint32_t const y, uint32_t const width, uint32_t const height) {

    auto viewport = D3D12_VIEWPORT {};
    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;

    _impl->current_buffer->command_list->RSSetViewports(1, &viewport);
}

void Backend::set_scissor(uint32_t const left, uint32_t const top, uint32_t const right, uint32_t const bottom) {

    auto rect = D3D12_RECT {};
    rect.top = static_cast<LONG>(top);
    rect.bottom = static_cast<LONG>(bottom);
    rect.left = static_cast<LONG>(left);
    rect.right = static_cast<LONG>(right);

    _impl->current_buffer->command_list->RSSetScissorRects(1, &rect);
}

void Backend::barrier(std::variant<Handle<Texture>, Handle<Buffer>> const& handle, MemoryState const before, MemoryState const after) {

    auto resource_barrier = D3D12_RESOURCE_BARRIER {};
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Handle<Texture>>)
            resource_barrier.Transition.pResource = _impl->textures.get(arg).resource.Get();
        else if constexpr (std::is_same_v<T, Handle<Buffer>>)
            resource_barrier.Transition.pResource = _impl->buffers.get(arg).resource.Get();
        else
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
    }, handle);

    auto get_memory_state = [&](MemoryState const state) -> D3D12_RESOURCE_STATES {
        switch(state) {
		    case MemoryState::Common: return D3D12_RESOURCE_STATE_COMMON;
		    case MemoryState::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		    case MemoryState::Present: return D3D12_RESOURCE_STATE_PRESENT;
		    //case MemoryState::GenericRead: return D3D12_RESOURCE_STATE_GENERIC_READ;
		    case MemoryState::CopySrc: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		    case MemoryState::CopyDst: return D3D12_RESOURCE_STATE_COPY_DEST;
            case MemoryState::PixelShaderResource: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        }
        return D3D12_RESOURCE_STATE_COMMON;
    };
    
    resource_barrier.Transition.StateBefore = get_memory_state(before);
    resource_barrier.Transition.StateAfter = get_memory_state(after);

    _impl->current_buffer->command_list->ResourceBarrier(1, &resource_barrier);
}

void Backend::begin_render_pass(Handle<RenderPass> const& handle, std::span<Color> const rtv_clears, float const depth_clear, uint8_t const stencil_clear) {

    auto& render_pass = _impl->render_passes.get(handle);

    for(uint32_t i = 0; i < rtv_clears.size(); ++i) {
        std::memcpy(render_pass.colors[i].BeginningAccess.Clear.ClearValue.Color, &rtv_clears[i], sizeof(Color));
    }

    if(render_pass.depth_stencil.has_value()) {

        render_pass.depth_stencil.value().DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = depth_clear;
        render_pass.depth_stencil.value().StencilBeginningAccess.Clear.ClearValue.DepthStencil.Stencil = stencil_clear;

        _impl->current_buffer->command_list->BeginRenderPass(
            static_cast<uint32_t>(render_pass.colors.size()), 
            render_pass.colors.data(), 
            &render_pass.depth_stencil.value(), 
            D3D12_RENDER_PASS_FLAG_NONE
        );
    } else {
        _impl->current_buffer->command_list->BeginRenderPass(
            static_cast<uint32_t>(render_pass.colors.size()), 
            render_pass.colors.data(), 
            nullptr, 
            D3D12_RENDER_PASS_FLAG_NONE
        );
    }
}

void Backend::end_render_pass() {

    _impl->current_buffer->command_list->EndRenderPass();
}

void Backend::resize_buffers(uint32_t const width, uint32_t const height, uint32_t const buffer_count) {

    /*for(uint32_t i = 0; i < _impl->frame_count; ++i) {
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

    _impl->frame_index = (_impl->frame_index + 1) % _impl->frame_count;*/
}

void Backend::bind_pipeline(Handle<Pipeline> const& handle) {

    auto& pipeline = _impl->pipelines.get(handle);

    _impl->current_buffer->current_pipeline = handle;

    _impl->current_buffer->command_list->SetPipelineState(pipeline.pipeline_state.Get());
    _impl->current_buffer->command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Backend::bind_vertex_buffer(uint32_t const index, Handle<Buffer> const& handle, uint64_t const offset) {

    auto& buffer = _impl->buffers.get(handle);
    auto& pipeline = _impl->pipelines.get(_impl->current_buffer->current_pipeline);

    auto vertex_view = D3D12_VERTEX_BUFFER_VIEW {};
    vertex_view.BufferLocation = buffer.resource->GetGPUVirtualAddress() + offset;
    vertex_view.SizeInBytes = static_cast<uint32_t>(buffer.resource->GetDesc().Width);
    vertex_view.StrideInBytes = pipeline.vertex_strides[index];

    _impl->current_buffer->command_list->IASetVertexBuffers(index, 1, &vertex_view);
}

void Backend::bind_index_buffer(Format const format, Handle<Buffer> const& handle, uint64_t const offset) {

    auto get_dxgi_format = [&](Format const format) {
        switch(format) {
            case Format::R32: return DXGI_FORMAT_R32_UINT;
        }
        return DXGI_FORMAT_R32_UINT;
    }; 

    auto& buffer = _impl->buffers.get(handle);

    auto index_view = D3D12_INDEX_BUFFER_VIEW {};
    index_view.BufferLocation = buffer.resource->GetGPUVirtualAddress() + offset;
    index_view.SizeInBytes = static_cast<uint32_t>(buffer.resource->GetDesc().Width);
    index_view.Format = get_dxgi_format(format);

    _impl->current_buffer->command_list->IASetIndexBuffer(&index_view);
}

void Backend::draw(uint32_t const vertex_index, uint32_t const vertex_count) {

    _impl->current_buffer->command_list->DrawInstanced(vertex_count, 1, vertex_index, 0);
}

void Backend::draw_indexed(uint32_t const index_count, uint32_t const instance_count, uint32_t const instance_offset) {

    _impl->current_buffer->command_list->DrawIndexedInstanced(index_count, instance_count, 0, 0, instance_offset);
}

void Backend::copy_texture_data(Handle<Texture> const& dst, std::span<char8_t const> const data) {

    auto& texture = _impl->textures.get(dst);
    D3D12_RESOURCE_DESC resource_desc = texture.resource->GetDesc();

    std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, 16> footprints;
    std::array<uint32_t, 16> num_rows;
    std::array<uint64_t, 16> row_sizes;

    size_t total_bytes = 0;

    _impl->device->GetCopyableFootprints(
        &resource_desc,
        0,
        1,
        0,
        footprints.data(),
        num_rows.data(),
        row_sizes.data(),
        &total_bytes
    );

    auto range = D3D12_RANGE {};
    char8_t* ptr;

    auto& stage_buffer = _impl->frames[_impl->frame_index].stage_buffer;

    // auto& layout = footprints[0];
    // const uint64_t subresource_pitch = layout.Footprint.RowPitch + (~layout.Footprint.RowPitch & D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1);

    THROW_IF_FAILED(stage_buffer.resource->Map(0, &range, reinterpret_cast<void**>(&ptr)));
    std::memcpy(ptr + stage_buffer.offset, data.data(), data.size());
    stage_buffer.resource->Unmap(0, &range);

    auto dst_location = D3D12_TEXTURE_COPY_LOCATION {};
    dst_location.pResource = texture.resource.Get();
    dst_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst_location.SubresourceIndex = 0;
 
    auto src_location = D3D12_TEXTURE_COPY_LOCATION {};
    src_location.pResource = stage_buffer.resource.Get();
    src_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src_location.PlacedFootprint = footprints[0];

    _impl->current_buffer->command_list->CopyTextureRegion(&dst_location, 0, 0, 0, &src_location, nullptr);
}

void Backend::copy_buffer_data(Handle<Buffer> const& handle, uint64_t const offset, std::span<char8_t> const data) {

    auto& buffer = _impl->buffers.get(handle);

    if(buffer.memory_alloc_info.heap->heap->GetDesc().Properties.Type == D3D12_HEAP_TYPE_UPLOAD) {

        auto range = D3D12_RANGE {};
        char8_t* ptr;
        THROW_IF_FAILED(buffer.resource->Map(0, &range, reinterpret_cast<void**>(&ptr)));
        std::memcpy(ptr + offset, data.data(), data.size());
        buffer.resource->Unmap(0, &range);
    }
}

void Backend::wait_for_idle_device() {

    for(uint32_t i = 0; i < static_cast<uint32_t>(_impl->frames.size()); ++i) {
        const uint64_t value = _impl->frames[i].direct_fence.fence_value;
        THROW_IF_FAILED(_impl->direct_queue->Signal(_impl->frames[i].direct_fence.fence.Get(), value));

        ++_impl->frames[i].direct_fence.fence_value;

        if(_impl->frames[i].direct_fence.fence->GetCompletedValue() < value) {
            THROW_IF_FAILED(_impl->frames[i].direct_fence.fence->SetEventOnCompletion(value, _impl->wait_event));
            WaitForSingleObjectEx(_impl->wait_event, INFINITE, false);
        }
    }
}

void Backend::begin_context(ContextType const context_type) {

    switch(context_type) {
        case ContextType::Graphics: {
            _impl->current_buffer = &_impl->frames[_impl->frame_index].direct_buffer; 

            THROW_IF_FAILED(_impl->current_buffer->command_allocator->Reset());
            THROW_IF_FAILED(_impl->current_buffer->command_list->Reset(_impl->current_buffer->command_allocator.Get(), nullptr));

            auto heaps = std::array<ID3D12DescriptorHeap*, 2> { 
                _impl->null_srv_descriptor_alloc_info.heap->heap.Get(), 
                _impl->null_sampler_descriptor_alloc_info.heap->heap.Get() 
            };
            _impl->current_buffer->command_list->SetDescriptorHeaps(2, heaps.data());
        } break;
        case ContextType::Copy: {
            _impl->current_buffer = &_impl->frames[_impl->frame_index].copy_buffer; 

            THROW_IF_FAILED(_impl->current_buffer->command_allocator->Reset());
            THROW_IF_FAILED(_impl->current_buffer->command_list->Reset(_impl->current_buffer->command_allocator.Get(), nullptr));
        } break;
        case ContextType::Compute: {
            _impl->current_buffer = &_impl->frames[_impl->frame_index].copy_buffer; 

            THROW_IF_FAILED(_impl->current_buffer->command_allocator->Reset());
            THROW_IF_FAILED(_impl->current_buffer->command_list->Reset(_impl->current_buffer->command_allocator.Get(), nullptr));
        } break;
    }
}

void Backend::end_context() {

    THROW_IF_FAILED(_impl->current_buffer->command_list->Close());
    _impl->current_buffer = nullptr;
}

void Backend::execute_context(ContextType const context_type) {

    switch(context_type) {
        case ContextType::Graphics: {
            _impl->direct_queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(_impl->frames[_impl->frame_index].direct_buffer.command_list.GetAddressOf()));
        } break;
        case ContextType::Copy: {
            _impl->copy_queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(_impl->frames[_impl->frame_index].copy_buffer.command_list.GetAddressOf()));
        } break;
        case ContextType::Compute: {
            _impl->compute_queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(_impl->frames[_impl->frame_index].compute_buffer.command_list.GetAddressOf()));
        } break;
    }
}

void Backend::wait_for_context(ContextType const context_type) {

    switch(context_type) {
        case ContextType::Graphics: {
            const uint64_t value = _impl->frames[_impl->frame_index].direct_fence.fence_value;
            THROW_IF_FAILED(_impl->direct_queue->Signal(_impl->frames[_impl->frame_index].direct_fence.fence.Get(), value));

            ++_impl->frames[_impl->frame_index].direct_fence.fence_value;

            if(_impl->frames[_impl->frame_index].direct_fence.fence->GetCompletedValue() < value) {
                THROW_IF_FAILED(_impl->frames[_impl->frame_index].direct_fence.fence->SetEventOnCompletion(value, _impl->wait_event));
                WaitForSingleObjectEx(_impl->wait_event, INFINITE, false);
            }
        } break;
        case ContextType::Copy: {
            const uint64_t value = _impl->frames[_impl->frame_index].copy_fence.fence_value;
            THROW_IF_FAILED(_impl->copy_queue->Signal(_impl->frames[_impl->frame_index].copy_fence.fence.Get(), value));

            ++_impl->frames[_impl->frame_index].copy_fence.fence_value;

            if(_impl->frames[_impl->frame_index].copy_fence.fence->GetCompletedValue() < value) {
                THROW_IF_FAILED(_impl->frames[_impl->frame_index].copy_fence.fence->SetEventOnCompletion(value, _impl->wait_event));
                WaitForSingleObjectEx(_impl->wait_event, INFINITE, false);
            }
        } break;
    }
}

bool Backend::is_finished_context(ContextType const context_type) {

    return true;
}