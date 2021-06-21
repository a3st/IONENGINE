// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

template<>
class DescriptorPool<DesctiptorType::Sampler> {
public:

    DescriptorPool(const Device& device, const uint32 descriptor_count, const bool shader_visibility) {

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        heap_desc.NumDescriptors = descriptor_count;
        if(shader_visibility) {
            heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        } else {
            heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        }
        device.m_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_descriptor_heap));
    }

private:

    ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
};

template<>
class DescriptorPool<DesctiptorType::RenderTarget> {
public:

    DescriptorPool(const Device& device, const uint32 descriptor_count, const bool shader_visibility) {

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heap_desc.NumDescriptors = descriptor_count;
        assert(!shader_visibility && "render target cannot be with flag shader_visible");
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        device.m_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_descriptor_heap));
    }

private:

    ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
};

template<>
class DescriptorPool<DesctiptorType::DepthStencil> {
public:

    DescriptorPool(const Device& device, const uint32 descriptor_count, const bool shader_visibility) {

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heap_desc.NumDescriptors = descriptor_count;
        assert(!shader_visibility && "depth stencil cannot be with flag shader_visible");
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        device.m_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_descriptor_heap));
    }

private:

    ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
};

template<>
class DescriptorPool<DesctiptorType::Buffer> {
public:

    DescriptorPool(const Device& device, const uint32 descriptor_count, const bool shader_visibility) {

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
        heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heap_desc.NumDescriptors = descriptor_count;
        if(shader_visibility) {
            heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        } else {
            heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        }
        device.m_device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_descriptor_heap));    
    }

private:

    ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
};
    
}