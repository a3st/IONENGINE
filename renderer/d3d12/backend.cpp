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

D3D12_HEAP_TYPE d3d12_heap_type(MemoryType const type) {
	
	switch(type) {
        case MemoryType::Default: return D3D12_HEAP_TYPE_DEFAULT;
        case MemoryType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
        case MemoryType::Readback: return D3D12_HEAP_TYPE_READBACK;
		default: assert(false && "passed invalid argument to d3d12_heap_type"); return D3D12_HEAP_TYPE_DEFAULT;
    }
}

D3D12_COMMAND_LIST_TYPE d3d12_command_list_type(CommandBufferType const type) {
	
	switch(type) {
        case CommandBufferType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandBufferType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandBufferType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		default: assert(false && "passed invalid argument to d3d12_command_list_type"); return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE d3d12_render_pass_begin_type(RenderPassLoadOp const op) {
	
	switch (op) {
    	case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to d3d12_render_pass_begin_type"); return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE d3d12_render_pass_end_type(RenderPassStoreOp const op) {
    
	switch (op) {
    	case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to d3d12_render_pass_end_type"); return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    }
}

DXGI_FORMAT dxgi_format(Format const format) {

	switch(format) {
		case Format::Unknown: return DXGI_FORMAT_UNKNOWN;
		case Format::RGBA32float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Format::RGBA32uint: return DXGI_FORMAT_R32G32B32A32_UINT;
		case Format::RGBA32int: return DXGI_FORMAT_R32G32B32A32_SINT;
		case Format::RGB32float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case Format::RGB32uint: return DXGI_FORMAT_R32G32B32_UINT;
		case Format::RGB32int: return DXGI_FORMAT_R32G32B32_SINT;
		case Format::RG32float: return DXGI_FORMAT_R32G32_FLOAT;
		case Format::RG32uint: return DXGI_FORMAT_R32G32_UINT;
		case Format::RG32int: return DXGI_FORMAT_R32G32_SINT;
		case Format::R32float: return DXGI_FORMAT_R32_FLOAT;
		case Format::R32uint: return DXGI_FORMAT_R32_UINT;
		case Format::R32int: return DXGI_FORMAT_R32_SINT;
		case Format::RGBA16float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case Format::RGBA16uint: return DXGI_FORMAT_R16G16B16A16_UINT;
		case Format::RGBA16int: return DXGI_FORMAT_R16G16B16A16_SINT;
		case Format::RGBA16unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case Format::RGBA16snorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
		case Format::RG16float: return DXGI_FORMAT_R16G16_FLOAT;
		case Format::RG16uint: return DXGI_FORMAT_R16G16_UINT;
		case Format::RG16int: return DXGI_FORMAT_R16G16_SINT;
		case Format::RG16unorm: return DXGI_FORMAT_R16G16_UNORM;
		case Format::RG16snorm: return DXGI_FORMAT_R16G16_SNORM;
		case Format::R16float: return DXGI_FORMAT_R16_FLOAT;
		case Format::R16uint: return DXGI_FORMAT_R16_UINT;
		case Format::R16int: return DXGI_FORMAT_R16_SINT;
		case Format::R16unorm: return DXGI_FORMAT_R16_UNORM;
		case Format::R16snorm: return DXGI_FORMAT_R16_SNORM;
		case Format::RGBA8uint: return DXGI_FORMAT_R8G8B8A8_UINT;
		case Format::RGBA8int: return DXGI_FORMAT_R8G8B8A8_SINT;
		case Format::RGBA8unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::RGBA8snorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
		case Format::RG8uint: return DXGI_FORMAT_R8G8_UINT;
		case Format::RG8int: return DXGI_FORMAT_R8G8_SINT;
		case Format::RG8unorm: return DXGI_FORMAT_R8G8_UNORM;
		case Format::RG8snorm: return DXGI_FORMAT_R8G8_SNORM;
		case Format::R8uint: return DXGI_FORMAT_R8_UINT;
		case Format::R8int: return DXGI_FORMAT_R8_SINT;
		case Format::R8unorm: return DXGI_FORMAT_R8_UNORM;
		case Format::R8snorm: return DXGI_FORMAT_R8_SNORM;
		default: assert(false && "passed invalid argument to dxgi_format"); return DXGI_FORMAT_UNKNOWN;
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

struct RenderPass::Impl {
    std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> colors;
    std::optional<D3D12_RENDER_PASS_DEPTH_STENCIL_DESC> depth_stencil;
    uint16_t sample_count;
};

RenderPass::RenderPass() = default;

RenderPass::RenderPass(
    Backend& backend, 
    std::span<RenderPassColorDesc const> const colors, 
    std::optional<RenderPassDepthStencilDesc> const depth_stencil, 
    uint16_t const sample_count
) : impl_(std::make_unique<Impl>()) {

    impl_->sample_count = sample_count;
    impl_->colors.reserve(colors.size());

    for(uint32_t i = 0; i < colors.size(); ++i) {
        D3D12_RENDER_PASS_BEGINNING_ACCESS begin{};
        begin.Type = d3d12_render_pass_begin_type(colors[i].load_op);
        begin.Clear.ClearValue.Format = dxgi_format(colors[i].format);

        D3D12_RENDER_PASS_ENDING_ACCESS end{};
        end.Type = d3d12_render_pass_end_type(colors[i].store_op);

        //impl_->colors.emplace_back();

        //render_pass_target_descs_[i].BeginningAccess = begin;
        //render_pass_target_descs_[i].EndingAccess = end;
    }
}

RenderPass::~RenderPass() = default;

struct CommandBuffer::Impl {
    Backend* backend;
    ComPtr<ID3D12GraphicsCommandList4> list;
    ComPtr<ID3D12CommandAllocator> allocator;
    CommandBufferType type;
};

CommandBuffer::CommandBuffer() = default;

CommandBuffer::CommandBuffer(Backend& backend, CommandBufferType const type) : impl_(std::make_unique<Impl>()) {

    impl_->type = type;
    impl_->backend = &backend;
    
    THROW_IF_FAILED(impl_->backend->impl_->device->CreateCommandAllocator(
        d3d12_command_list_type(type), 
        __uuidof(ID3D12CommandAllocator), 
        reinterpret_cast<void**>(impl_->allocator.GetAddressOf())
    ));

    THROW_IF_FAILED(impl_->backend->impl_->device->CreateCommandList(
        0, 
        d3d12_command_list_type(type), 
        impl_->allocator.Get(), 
        nullptr, 
        __uuidof(ID3D12GraphicsCommandList4), 
        reinterpret_cast<void**>(impl_->list.GetAddressOf())
    ));

    THROW_IF_FAILED(impl_->list->Close());
}

void CommandBuffer::reset() {

    THROW_IF_FAILED(impl_->allocator->Reset());
    THROW_IF_FAILED(impl_->list->Reset(impl_->allocator.Get(), nullptr));
}

void CommandBuffer::close() {

    THROW_IF_FAILED(impl_->list->Close());
}

void CommandBuffer::begin_render_pass() {

    
}

void CommandBuffer::end_render_pass() {

    impl_->list->EndRenderPass();
}

CommandBuffer::CommandBuffer(CommandBuffer&&) = default;

CommandBuffer& CommandBuffer::operator=(CommandBuffer&&) = default;

CommandBuffer::~CommandBuffer() = default;