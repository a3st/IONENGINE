// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DView : public View {
public:

    D3DView(ID3D12Device4* d3d12_device, const ViewType view_type, D3DResource* resource, const ViewDesc& view_desc) 
        : m_d3d12_device(d3d12_device), m_resource(resource), m_type(view_type), m_desc(view_desc) {

        //uint64 offset = m_descriptor_pool.get().d3d12_allocate(convert_descriptor_heap_type(view_desc.view_type));
        //m_d3d12_cpu_descriptor = m_descriptor_pool.get().get_d3d12_cpu_descriptor_handle(convert_descriptor_heap_type(view_desc.view_type), offset);
        
        switch(m_type) {

            case ViewType::RenderTarget: {
                /*auto& resource_desc = std::get<D3D12_RESOURCE_DESC>(m_resource.get().get_d3d12_desc());

                D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
                rtv_desc.Format = resource_desc.Format;
                
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

                m_d3d12_device.get()->CreateRenderTargetView(m_resource.get().get_d3d12_resource().get(), &rtv_desc, m_d3d12_cpu_descriptor);*/
                break;
            }
            case ViewType::DepthStencil: {
                /*auto& resource_desc = std::get<D3D12_RESOURCE_DESC>(m_resource.get().get_d3d12_desc());

                D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
                dsv_desc.Format = resource_desc.Format;
                
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

                m_d3d12_device.get()->CreateDepthStencilView(m_resource.get().get_d3d12_resource().get(), &dsv_desc, m_d3d12_cpu_descriptor);*/
                break;
            }
            case ViewType::ConstantBuffer: {
                /*auto& resource_desc = std::get<D3D12_RESOURCE_DESC>(m_resource.get().get_d3d12_desc());

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_view{};
                cbv_view.BufferLocation = m_resource.get().get_d3d12_resource()->GetGPUVirtualAddress();
                cbv_view.SizeInBytes = static_cast<uint32>(m_view_desc.buffer_size);

                m_d3d12_device.get()->CreateConstantBufferView(&cbv_view, m_d3d12_cpu_descriptor);*/
                break;
            }
        }
    }

    Resource* get_resource() const override { return m_resource; }

private:

    ID3D12Device4* m_d3d12_device;

    D3DResource* m_resource;

    ViewType m_type;
    ViewDesc m_desc;
};

}