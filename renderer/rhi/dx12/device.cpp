// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "device.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "command_buffer.hpp"
#include "allocator.hpp"
#include "utils.hpp"
#include "core/exception.hpp"
#include "platform/window.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

DX12Device::DX12Device(platform::Window const& window) {

    uint32_t factory_flags = 0;

#ifdef _DEBUG
    // Create Debug Layer in debug engine build
    {
        THROW_IF_FAILED(::D3D12GetDebugInterface(__uuidof(ID3D12Debug), debug.put_void()));
        debug->EnableDebugLayer();

        // Factory debug flags
        factory_flags = DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    THROW_IF_FAILED(::CreateDXGIFactory2(factory_flags, __uuidof(IDXGIFactory4), factory.put_void()));

    {
        winrt::com_ptr<IDXGIAdapter1> adapter;

        for(uint32_t i = 0; factory->EnumAdapters1(i, adapter.put()) != DXGI_ERROR_NOT_FOUND; ++i) {

            auto dxgi_adapter_desc = DXGI_ADAPTER_DESC1 {};
            THROW_IF_FAILED(adapter->GetDesc1(&dxgi_adapter_desc));

            if(dxgi_adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            } else {
                std::wcout << dxgi_adapter_desc.Description << std::endl;
                this->adapter = adapter;
                break;
            }
        }
    }

    THROW_IF_FAILED(::D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4), device.put_void()));

    // Get queues for device
    auto queue_types = std::array<D3D12_COMMAND_LIST_TYPE, 3> {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_TYPE_COPY,
        D3D12_COMMAND_LIST_TYPE_COMPUTE
    };

    for(auto const queue_type : queue_types)
    {
        winrt::com_ptr<ID3D12CommandQueue> queue;
        {
            auto d3d12_queue_desc = D3D12_COMMAND_QUEUE_DESC {};
            d3d12_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            d3d12_queue_desc.Type = queue_type;

            THROW_IF_FAILED(device->CreateCommandQueue(
                &d3d12_queue_desc,
                __uuidof(ID3D12CommandQueue), 
                queue.put_void())
            );
        }

        winrt::com_ptr<ID3D12Fence> fence;
        {
            THROW_IF_FAILED(device->CreateFence(
                0, 
                D3D12_FENCE_FLAG_NONE, 
                __uuidof(ID3D12Fence), 
                fence.put_void())
            );
        }

        auto queue_info = QueueInfo {
            .queue = queue,
            .fence = fence,
            .fence_value = 0
        };
        queue_infos.emplace_back(queue_info);
    }

    // Create WINAPI fence object
    {
        fence_event = CreateEvent(nullptr, false, false, nullptr);
        if(!fence_event) {
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    pool_allocator = core::make_ref<PoolDescriptorAllocator>(device.get(), true);
    command_allocator = core::make_ref<CommandAllocator>(device.get());
    upload_context = core::make_ref<UploadContext>(
        device.get(), 
        queue_infos[1].queue.get(), 
        queue_infos[1].fence.get(), 
        queue_infos[1].fence_value,
        &command_allocator,
        32 * 1024 * 1024
    );

    // Create swapchain using device queue
    {
        auto dxgi_swapchain_desc = DXGI_SWAP_CHAIN_DESC1 {};
        dxgi_swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dxgi_swapchain_desc.Width = window.get_width();
        dxgi_swapchain_desc.Height = window.get_height();
        dxgi_swapchain_desc.BufferCount = 2;
        dxgi_swapchain_desc.SampleDesc.Count = 1;
        dxgi_swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dxgi_swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        dxgi_swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        {
            winrt::com_ptr<IDXGISwapChain1> swapchain;
            THROW_IF_FAILED(factory->CreateSwapChainForHwnd(
                queue_infos[0].queue.get(), 
                reinterpret_cast<HWND>(window.get_native_handle()), 
                &dxgi_swapchain_desc, 
                nullptr, 
                nullptr, 
                swapchain.put())
            );
            swapchain.as(this->swapchain);
        }

        for(uint32_t i = 0; i < dxgi_swapchain_desc.BufferCount; ++i) {
            winrt::com_ptr<ID3D12Resource> resource;
            THROW_IF_FAILED(swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));

            auto texture = core::make_ref<DX12Texture>(device.get(), resource, &pool_allocator, (TextureUsageFlags)TextureUsage::RenderTarget);
            
            auto frame_info = FrameInfo {
                .swapchain_buffer = texture,
                .command_allocator = core::make_ref<CommandAllocator>(device.get())
            };
            frame_infos.emplace_back(frame_info);
        }
    }

    command_batches.reserve(64);
}

DX12Device::~DX12Device() {

    wait_for_idle();
}

auto DX12Device::create_allocator(size_t const block_size, size_t const shrink_size, BufferUsageFlags const flags) -> core::ref_ptr<MemoryAllocator> {

    return core::make_ref<DX12MemoryAllocator>(device.get(), block_size, shrink_size, flags);
}

auto DX12Device::allocate_command_buffer(CommandBufferType const buffer_type) -> core::ref_ptr<CommandBuffer> {

    D3D12_COMMAND_LIST_TYPE list_type;
    switch(buffer_type) {
        case CommandBufferType::Graphics: {
            list_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        } break;
        case CommandBufferType::Copy: {
            list_type = D3D12_COMMAND_LIST_TYPE_COPY;
        } break;
        case CommandBufferType::Compute: {
            list_type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        } break;
        default: {
            assert(false && "Unsupported command buffer type for allocation");
        } break;
    }

    auto command_buffer = frame_infos[frame_index].command_allocator->allocate(list_type);
    static_cast<DX12CommandBuffer*>(command_buffer.get())->reset();
    return command_buffer;
}

auto DX12Device::create_texture() -> Future<Texture> {

    return Future<DX12Texture>();
}

auto DX12Device::create_buffer(
    MemoryAllocator& allocator, 
    size_t const size, 
    BufferUsageFlags const flags, 
    std::span<uint8_t const> const data
) -> Future<Buffer> 
{
    auto buffer = core::make_ref<DX12Buffer>(
        device.get(),
        allocator,
        &pool_allocator,
        size,
        flags
    );

    upload_context->upload(static_cast<DX12Buffer*>(buffer.get())->get_resource(), data);

    return Future<DX12Buffer>(buffer);
}

auto DX12Device::write_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, std::span<uint8_t const> const data) -> Future<Buffer> {

    auto buffer2 = core::make_ref<DX12Buffer>();

    return Future<DX12Buffer>(buffer2);
}

auto DX12Device::submit_command_lists(std::span<core::ref_ptr<CommandBuffer>> const command_buffers) -> void {

    for(auto const& command_buffer : command_buffers) {
        command_batches.emplace_back(reinterpret_cast<ID3D12CommandList*>(static_cast<DX12CommandBuffer*>(command_buffer.get())->get_command_list()));
    }

    uint32_t queue_index = 0;
    switch(static_cast<DX12CommandBuffer*>(command_buffers[0].get())->get_list_type()) {
        case D3D12_COMMAND_LIST_TYPE_DIRECT: {
            queue_index = 0;
        } break;
        case D3D12_COMMAND_LIST_TYPE_COPY: {
            queue_index = 1;
        } break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: {
            queue_index = 2;
        } break;
        default: {
            assert(false && "Unsupported command buffer type to submit");
        } break;
    }

    queue_infos[queue_index].queue->ExecuteCommandLists(static_cast<uint32_t>(command_batches.size()), command_batches.data());
    command_batches.clear();

    queue_infos[queue_index].fence_value++;
    THROW_IF_FAILED(queue_infos[queue_index].queue->Signal(queue_infos[queue_index].fence.get(), queue_infos[queue_index].fence_value));
}

auto DX12Device::wait_for_idle() -> void {

    for(auto& queue_info : queue_infos) {
        queue_info.fence_value++;
        THROW_IF_FAILED(queue_info.queue->Signal(queue_info.fence.get(), queue_info.fence_value));
        THROW_IF_FAILED(queue_info.fence->SetEventOnCompletion(queue_info.fence_value, fence_event));
        ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
    }
}

auto DX12Device::request_next_swapchain_buffer() -> core::ref_ptr<Texture> {

    frame_index = swapchain->GetCurrentBackBufferIndex();
    frame_infos[frame_index].command_allocator->reset();
    return frame_infos[frame_index].swapchain_buffer;
}

auto DX12Device::present() -> void {

    uint64_t const prev_fence_value = frame_infos[(frame_index + 1) % frame_infos.size()].present_fence_value;

    if(queue_infos[0].fence->GetCompletedValue() < prev_fence_value) {
        THROW_IF_FAILED(queue_infos[0].fence->SetEventOnCompletion(prev_fence_value, fence_event));
        ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
    }

    frame_infos[frame_index].present_fence_value = queue_infos[0].fence_value;
    
    THROW_IF_FAILED(swapchain->Present(0, 0));
}

auto DX12Device::resize_swapchain_buffers(uint32_t const width, uint32_t const height) -> void {

    wait_for_idle();

    for(auto& frame_info : frame_infos) {
        frame_info.swapchain_buffer = nullptr;
    }

    auto dxgi_swapchain_desc = DXGI_SWAP_CHAIN_DESC1 {};
    THROW_IF_FAILED(swapchain->GetDesc1(&dxgi_swapchain_desc));

    THROW_IF_FAILED(swapchain->ResizeBuffers(dxgi_swapchain_desc.BufferCount, width, height, dxgi_swapchain_desc.Format, 0));

    for(uint32_t i = 0; i < dxgi_swapchain_desc.BufferCount; ++i) {
        winrt::com_ptr<ID3D12Resource> resource;
        THROW_IF_FAILED(swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));

        auto texture = core::make_ref<DX12Texture>(device.get(), resource, &pool_allocator, (TextureUsageFlags)TextureUsage::RenderTarget);
        frame_infos[i].swapchain_buffer = texture;
    }
}
