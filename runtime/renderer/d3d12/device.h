// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Device final {
public:

    Device(const Adapter& adapter) : m_adapter(adapter) {
        
        throw_if_failed(D3D12CreateDevice(m_adapter.get_adapter_ptr().Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device_ptr)));

        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        throw_if_failed(m_device_ptr->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queue_ptrs.direct)));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        throw_if_failed(m_device_ptr->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queue_ptrs.copy)));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        throw_if_failed(m_device_ptr->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queue_ptrs.compute)));

        m_descriptor_sizes.srv = m_device_ptr->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_descriptor_sizes.dsv = m_device_ptr->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_descriptor_sizes.rtv = m_device_ptr->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_descriptor_sizes.sampler = m_device_ptr->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

        m_device_ptr->SetName(L"D3D12 Device Object");
    }

    const ComPtr<ID3D12Device4>& get_device_ptr() const { return m_device_ptr; }
    const ComPtr<IDXGIAdapter4>& get_adapter_ptr() const { return m_adapter.get_adapter_ptr(); }
    const ComPtr<ID3D12CommandQueue>& get_queue_direct_ptr() const { return m_queue_ptrs.direct; }
    const ComPtr<ID3D12CommandQueue>& get_queue_copy_ptr() const { return m_queue_ptrs.copy; }
    const ComPtr<ID3D12CommandQueue>& get_queue_compute_ptr() const { return m_queue_ptrs.compute; }

private:

    const Adapter& m_adapter;

    ComPtr<ID3D12Device4> m_device_ptr;
    struct {
        ComPtr<ID3D12CommandQueue> direct;
        ComPtr<ID3D12CommandQueue> copy;
        ComPtr<ID3D12CommandQueue> compute;
    } m_queue_ptrs;

    struct {
        uint32 srv;
        uint32 rtv;
        uint32 dsv;
        uint32 sampler;
    } m_descriptor_sizes;
};

}