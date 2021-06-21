// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class DeviceLimits : uint32 {
    Sampler = 16,
    Buffer = 12,
    Resource = 128,
    UnorderedAccess = 12
};

class Device final {
friend class Swapchain;
friend class DescriptorPool<DesctiptorType::Sampler>;
friend class DescriptorPool<DesctiptorType::RenderTarget>;
friend class DescriptorPool<DesctiptorType::DepthStencil>;
friend class DescriptorPool<DesctiptorType::Buffer>;
public:

    Device(const Adapter& adapter) : m_adapter(adapter) {
        
        throw_if_failed(D3D12CreateDevice(m_adapter.m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

        D3D12_COMMAND_QUEUE_DESC direct_desc = UTILS_COMMAND_QUEUE_DESC::as_direct_queue(D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, D3D12_COMMAND_QUEUE_FLAG_NONE);
        D3D12_COMMAND_QUEUE_DESC copy_desc = UTILS_COMMAND_QUEUE_DESC::as_copy_queue(D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, D3D12_COMMAND_QUEUE_FLAG_NONE);
        D3D12_COMMAND_QUEUE_DESC compute_desc = UTILS_COMMAND_QUEUE_DESC::as_compute_queue(D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, D3D12_COMMAND_QUEUE_FLAG_NONE);

        throw_if_failed(m_device->CreateCommandQueue(&direct_desc, IID_PPV_ARGS(&m_queues.direct)));
        throw_if_failed(m_device->CreateCommandQueue(&copy_desc, IID_PPV_ARGS(&m_queues.copy)));
        throw_if_failed(m_device->CreateCommandQueue(&compute_desc, IID_PPV_ARGS(&m_queues.compute)));

        m_descriptor_sizes.srv = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_descriptor_sizes.dsv = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_descriptor_sizes.rtv = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_descriptor_sizes.sampler = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

        m_device->SetName(L"D3D12 Device Object");
    }

private:

    const Adapter& m_adapter;
    ComPtr<ID3D12Device4> m_device;

    struct {
        ComPtr<ID3D12CommandQueue> direct;
        ComPtr<ID3D12CommandQueue> copy;
        ComPtr<ID3D12CommandQueue> compute;
    } m_queues;

    struct {
        uint32 srv;
        uint32 rtv;
        uint32 dsv;
        uint32 sampler;
    } m_descriptor_sizes;
};

}