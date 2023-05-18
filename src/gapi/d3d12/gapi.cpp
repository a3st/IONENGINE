// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "gapi/d3d12/gapi.hpp"
#include "core/string_utils.hpp"

using namespace ie::gapi;

#define THROW_HRESULT_IF_FAILED(HRESULT) if(FAILED(HRESULT))\
{ throw std::runtime_error(std::format("An error occurred while processing the WINAPI (HRESULT: {:04x})", HRESULT)); } 

Instance::Instance(bool const debug_) : debug(debug_) {

    uint32_t const flags = debug ? DXGI_CREATE_FACTORY_DEBUG : 0;

    THROW_HRESULT_IF_FAILED(::CreateDXGIFactory2(
        flags, 
        __uuidof(factory), 
        factory.put_void())
    );
}

auto Instance::get_adapters() -> std::vector<AdapterDesc> {
    std::vector<AdapterDesc> out;

    winrt::com_ptr<IDXGIAdapter> dxgi_adapter;
    for(uint32_t i = 0; factory->EnumAdapters(i, dxgi_adapter.put()) != DXGI_ERROR_NOT_FOUND; ++i) {
        auto dxgi_adapter4 = dxgi_adapter.try_as<IDXGIAdapter4>();

        auto dxgi_adapter_desc = DXGI_ADAPTER_DESC3 {};
        THROW_HRESULT_IF_FAILED(dxgi_adapter4->GetDesc3(&dxgi_adapter_desc));

        if(dxgi_adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            dxgi_adapter = nullptr;
            continue;
        }

        winrt::com_ptr<ID3D12Device> d3d12_device;
        if(::D3D12CreateDevice(
            dxgi_adapter.get(), 
            D3D_FEATURE_LEVEL_12_0, 
            __uuidof(d3d12_device), 
            d3d12_device.put_void()) != S_OK
        ) {
            dxgi_adapter = nullptr;
            continue;
        }

        auto d3d12_feature_data = D3D12_FEATURE_DATA_ARCHITECTURE1 {};
        THROW_HRESULT_IF_FAILED(d3d12_device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &d3d12_feature_data, sizeof(d3d12_feature_data)));

        auto adapter_desc = AdapterDesc {
            .index = static_cast<uint16_t>(i),
            .name = core::string_convert(std::wstring_view(dxgi_adapter_desc.Description)),
            .memory_size = dxgi_adapter_desc.DedicatedVideoMemory,
            .is_uma = d3d12_feature_data.UMA == 1 ? true : false
        };

        out.push_back(std::move(adapter_desc));
        dxgi_adapter = nullptr;
    }
    return out;
}

Device::Device(
    Instance& instance, 
    uint16_t const adapter_index, 
    core::ref_ptr<platform::Window> window_
) : window(window_) 
{
    winrt::com_ptr<IDXGIAdapter> adapter;
    THROW_HRESULT_IF_FAILED(instance.factory->EnumAdapters(adapter_index, adapter.put()));

    THROW_HRESULT_IF_FAILED(::D3D12CreateDevice(
        adapter.get(), 
        D3D_FEATURE_LEVEL_12_0, 
        __uuidof(device), 
        device.put_void())
    );

    // Direct Command Queue
    {
        auto d3d12_command_queue = D3D12_COMMAND_QUEUE_DESC {};
        d3d12_command_queue.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        THROW_HRESULT_IF_FAILED(device->CreateCommandQueue(
            &d3d12_command_queue, 
            __uuidof(direct_queue), 
            direct_queue.put_void())
        );
    }

    // Copy Command Queue
    {
        auto d3d12_command_queue = D3D12_COMMAND_QUEUE_DESC {};
        d3d12_command_queue.Type = D3D12_COMMAND_LIST_TYPE_COPY;

        THROW_HRESULT_IF_FAILED(device->CreateCommandQueue(
            &d3d12_command_queue, 
            __uuidof(direct_queue), 
            copy_queue.put_void())
        );
    }

    // Compute Command Queue
    {
        auto d3d12_command_queue = D3D12_COMMAND_QUEUE_DESC {};
        d3d12_command_queue.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

        THROW_HRESULT_IF_FAILED(device->CreateCommandQueue(
            &d3d12_command_queue, 
            __uuidof(direct_queue), 
            compute_queue.put_void())
        );
    }

    if(window) {
        auto dxgi_swap_chain = DXGI_SWAP_CHAIN_DESC {};
        dxgi_swap_chain.BufferCount = 2;
        dxgi_swap_chain.OutputWindow = window->get_handle();
        dxgi_swap_chain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        dxgi_swap_chain.SampleDesc.Count = 1;
        dxgi_swap_chain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dxgi_swap_chain.Windowed = TRUE;

        THROW_HRESULT_IF_FAILED(instance.factory->CreateSwapChain(
            direct_queue.get(), 
            &dxgi_swap_chain, 
            swapchain.put()
        ));
    }
}