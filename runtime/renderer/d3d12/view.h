// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DView : public View {
public:

    D3DView(winrt::com_ptr<ID3D12Device4>& device, D3DDescriptorPool& descriptor_pool, D3DResource& resource, const ViewDesc& view_desc) : 
        m_d3d12_device(device), m_descriptor_pool(descriptor_pool), m_resource(resource), m_view_desc(view_desc) {

        uint64 offset = m_descriptor_pool.get().allocate(convert_descriptor_heap_type(view_desc.view_type));
        m_cpu_descriptor = m_descriptor_pool.get().get_cpu_descriptor_handle(convert_descriptor_heap_type(view_desc.view_type), offset);
        
        switch(view_desc.view_type) {

            case ViewType::RenderTarget: {

                D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
                rtv_desc.Format = std::get<D3D12_RESOURCE_DESC>(m_resource.get().get_desc()).Format;
                
                switch(view_desc.dimension) {

                    case ViewDimension::Texture1D: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                        rtv_desc.Texture1D.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                    case ViewDimension::Texture1DArray: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                        rtv_desc.Texture1DArray.FirstArraySlice = view_desc.base_array_layer;
                        rtv_desc.Texture1DArray.ArraySize = view_desc.layer_count;
                        rtv_desc.Texture1DArray.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                    case ViewDimension::Texture2D: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                        rtv_desc.Texture2D.PlaneSlice = view_desc.plane_slice;
                        rtv_desc.Texture2D.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                    case ViewDimension::Texture2DArray: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                        rtv_desc.Texture2DArray.FirstArraySlice = view_desc.base_array_layer;
                        rtv_desc.Texture2DArray.ArraySize = view_desc.layer_count;
                        rtv_desc.Texture2DArray.MipSlice = view_desc.base_mip_level;
                        rtv_desc.Texture2DArray.PlaneSlice = view_desc.plane_slice;
                        break;
                    }
                    case ViewDimension::Texture3D: {
                        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                        rtv_desc.Texture3D.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                }

                m_d3d12_device.get()->CreateRenderTargetView(m_resource.get().get_resource().get(), &rtv_desc, m_cpu_descriptor);
                break;
            }

            case ViewType::DepthStencil: {

                D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
                dsv_desc.Format = std::get<D3D12_RESOURCE_DESC>(m_resource.get().get_desc()).Format;
                
                switch(view_desc.dimension) {

                    case ViewDimension::Texture1D: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
                        dsv_desc.Texture1D.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                    case ViewDimension::Texture1DArray: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
                        dsv_desc.Texture1DArray.FirstArraySlice = view_desc.base_array_layer;
                        dsv_desc.Texture1DArray.ArraySize = view_desc.layer_count;
                        dsv_desc.Texture1DArray.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                    case ViewDimension::Texture2D: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                        dsv_desc.Texture2D.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                    case ViewDimension::Texture2DArray: {
                        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                        dsv_desc.Texture2DArray.FirstArraySlice = view_desc.base_array_layer;
                        dsv_desc.Texture2DArray.ArraySize = view_desc.layer_count;
                        dsv_desc.Texture2DArray.MipSlice = view_desc.base_mip_level;
                        break;
                    }
                }

                m_d3d12_device.get()->CreateDepthStencilView(m_resource.get().get_resource().get(), &dsv_desc, m_cpu_descriptor);
                break;
            }

            case ViewType::ConstantBuffer: {

                break;
            }
        }
    }

    Resource& get_resource() const override { return m_resource; }

    D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_descriptor() const { return m_cpu_descriptor; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_d3d12_device;
    std::reference_wrapper<D3DDescriptorPool> m_descriptor_pool;
    std::reference_wrapper<D3DResource> m_resource;

    ViewDesc m_view_desc;

    D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_descriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_descriptor;
};

}