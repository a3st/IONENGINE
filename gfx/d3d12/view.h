// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<>
class View<backend::d3d12> {
public:

    View(ID3D12Device4* d3d12_device, const ViewType view_type, Resource<backend::d3d12>* resource, const ViewDesc& view_desc) 
        : m_d3d12_device(d3d12_device), m_type(view_type), m_desc(view_desc) {

        assert(d3d12_device && "pointer to d3d12_device is null");
        assert(resource && "pointer to resource is null");

        m_descriptor_ptr = DescriptorAllocatorWrapper<backend::d3d12>::allocate(view_type);
        
        switch(m_type) {

            case ViewType::RenderTarget: {
                D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};

                auto& d3d12_resource_desc = std::get<D3D12_RESOURCE_DESC>(resource->get_d3d12_desc());
                rtv_desc.Format = d3d12_resource_desc.Format;
                
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

                m_d3d12_device->CreateRenderTargetView(resource->get_d3d12_resource(), &rtv_desc, cpu_handle);
                break;
            }
            case ViewType::DepthStencil: {
                D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};

                auto& d3d12_resource_desc = std::get<D3D12_RESOURCE_DESC>(resource->get_d3d12_desc());
                dsv_desc.Format = d3d12_resource_desc.Format;
                
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

                m_d3d12_device->CreateDepthStencilView(resource->get_d3d12_resource(), &dsv_desc, cpu_handle);
                break;
            }
            case ViewType::ConstantBuffer: {
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_view{};
                cbv_view.BufferLocation = resource->get_d3d12_resource()->GetGPUVirtualAddress();
                cbv_view.SizeInBytes = static_cast<uint32>(m_desc.buffer_size);

                D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
                    m_descriptor_ptr.heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                        m_descriptor_ptr.offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(m_type))
                };

                m_d3d12_device->CreateConstantBufferView(&cbv_view, cpu_handle);
                break;
            }
            case ViewType::Sampler: {
                D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { 
                    m_descriptor_ptr.heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                        m_descriptor_ptr.offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(m_type))
                };

                auto& d3d12_sampler_desc = std::get<D3D12_SAMPLER_DESC>(resource->get_d3d12_desc());
                m_d3d12_device->CreateSampler(&d3d12_sampler_desc, cpu_handle);
                break;
            }
        }
    }

    ~View() {

        if(m_descriptor_ptr.heap) {
            DescriptorAllocatorWrapper<backend::d3d12>::deallocate(m_type, m_descriptor_ptr);
        }
    }

    ViewType get_type() const { return m_type; }

    const ViewDesc& get_desc() const { return m_desc; }

    const D3DDescriptorPtr& get_descriptor_ptr() const { return m_descriptor_ptr; }

private:

    ID3D12Device4* m_d3d12_device;

    D3DDescriptorPtr m_descriptor_ptr;

    ViewType m_type;

    ViewDesc m_desc;
};

}