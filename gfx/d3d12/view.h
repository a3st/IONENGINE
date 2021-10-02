// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DView : public View {
public:

    D3DView(ID3D12Device4* d3d12_device, const ViewType view_type, D3DResource* resource, const ViewDesc& view_desc) 
        : m_d3d12_device(d3d12_device), m_resource(resource), m_type(view_type), m_desc(view_desc) {

        assert(d3d12_device && "pointer to d3d12_device is null");
        assert(resource && "pointer to resource is null");

        m_descriptor_ptr = D3DDescriptorAllocatorWrapper::allocate(view_type);
        
        switch(m_type) {

            case ViewType::RenderTarget: {
                D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
                rtv_desc.Format = resource->get_d3d12_desc().Format;
                
                switch(view_desc.dimension) {
                    case ViewDimension::Texture1D: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                        rtv_desc.Texture1D.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    case ViewDimension::Texture1DArray: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                        rtv_desc.Texture1DArray.FirstArraySlice = view_desc.array_slice;
                        rtv_desc.Texture1DArray.ArraySize = view_desc.array_size;
                        rtv_desc.Texture1DArray.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    case ViewDimension::Texture2D: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                        rtv_desc.Texture2D.PlaneSlice = view_desc.plane_slice;
                        rtv_desc.Texture2D.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    case ViewDimension::Texture2DArray: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                        rtv_desc.Texture2DArray.FirstArraySlice = view_desc.array_slice;
                        rtv_desc.Texture2DArray.ArraySize = view_desc.array_size;
                        rtv_desc.Texture2DArray.MipSlice = view_desc.mip_slice;
                        rtv_desc.Texture2DArray.PlaneSlice = view_desc.plane_slice;
                        break;
                    }
                    case ViewDimension::Texture3D: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                        rtv_desc.Texture3D.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    default: assert(false && "passed unsupported view dimension"); break;
                }

                D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
                    m_descriptor_ptr.heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                        m_descriptor_ptr.offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(m_type))
                };

                m_d3d12_device->CreateRenderTargetView(m_resource->get_d3d12_resource(), &rtv_desc, cpu_handle);
                break;
            }
            case ViewType::DepthStencil: {
                D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
                dsv_desc.Format = resource->get_d3d12_desc().Format;
                
                switch(view_desc.dimension) {
                    case ViewDimension::Texture1D: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
                        dsv_desc.Texture1D.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    case ViewDimension::Texture1DArray: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
                        dsv_desc.Texture1DArray.FirstArraySlice = view_desc.array_slice;
                        dsv_desc.Texture1DArray.ArraySize = view_desc.array_size;
                        dsv_desc.Texture1DArray.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    case ViewDimension::Texture2D: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                        dsv_desc.Texture2D.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    case ViewDimension::Texture2DArray: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                        dsv_desc.Texture2DArray.FirstArraySlice = view_desc.array_slice;
                        dsv_desc.Texture2DArray.ArraySize = view_desc.array_size;
                        dsv_desc.Texture2DArray.MipSlice = view_desc.mip_slice;
                        break;
                    }
                    default: assert(false && "passed unsupported view dimension"); break;
                }

                D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
                    m_descriptor_ptr.heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                        m_descriptor_ptr.offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(m_type))
                };

                m_d3d12_device->CreateDepthStencilView(m_resource->get_d3d12_resource(), &dsv_desc, cpu_handle);
                break;
            }
            case ViewType::ConstantBuffer: {
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_view{};
                cbv_view.BufferLocation = m_resource->get_d3d12_resource()->GetGPUVirtualAddress();
                cbv_view.SizeInBytes = static_cast<uint32>(m_desc.buffer_size);

                D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
                    m_descriptor_ptr.heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                        m_descriptor_ptr.offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(m_type))
                };

                m_d3d12_device->CreateConstantBufferView(&cbv_view, cpu_handle);
                break;
            }
        }
    }

    D3DView(ID3D12Device4* d3d12_device, D3DSampler* sampler) 
        : m_d3d12_device(d3d12_device), m_sampler(sampler), m_type(ViewType::Sampler) {

        assert(d3d12_device && "pointer to d3d12_device is null");
        assert(sampler && "pointer to sampler is null");

        m_descriptor_ptr = D3DDescriptorAllocatorWrapper::allocate(m_type);

        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
            m_descriptor_ptr.heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                m_descriptor_ptr.offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(m_type))
        };

        m_d3d12_device->CreateSampler(&sampler->get_d3d12_desc(), cpu_handle);
    }

    ~D3DView() {

        if(m_descriptor_ptr.heap) {
            D3DDescriptorAllocatorWrapper::deallocate(m_type, m_descriptor_ptr);
        }
    }

    ViewType get_type() const override { return m_type; }

    const ViewDesc& get_desc() const override { return m_desc; }

    Resource* get_resource() const override { return m_resource; }

    Sampler* get_sampler() const override { return m_sampler; }

    const D3DDescriptorPtr& get_descriptor_ptr() const { return m_descriptor_ptr; }

private:

    ID3D12Device4* m_d3d12_device;

    D3DResource* m_resource;
    D3DSampler* m_sampler;

    D3DDescriptorPtr m_descriptor_ptr;

    ViewType m_type;
    ViewDesc m_desc;
};

}