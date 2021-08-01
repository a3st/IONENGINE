// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

struct SubresourceRange final {

};

struct ComponentMapping final {
    ComponentSwizzleType r;
    ComponentSwizzleType g;
    ComponentSwizzleType b;
    ComponentSwizzleType a;
};

class Image {
friend class ImageView;
friend class FramebufferAttachment;
public:

    Image(Device& device, const ImageType image_type, uint32 mip_levels, const ImageFormat image_format) {
        
        m_desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(image_type);
        m_desc.Format = static_cast<DXGI_FORMAT>(image_format);
        m_desc.MipLevels = static_cast<uint16>(mip_levels);
    }

    Image(const Image&) = delete;

    Image(Image&& rhs) noexcept {

    }

    Image& operator=(const Image&) = delete;

    Image& operator=(Image&& rhs) noexcept {

    }

private:

    ComPtr<ID3D12Resource> m_ptr;

    D3D12_RESOURCE_DESC m_desc;
};

class ImageView {
public:

    ImageView(Device& device, Image& image, const ImageViewType view_type, const ImageFormat view_format, const ComponentMapping& components, const SubresourceRange& subresources) :
        m_device(device), m_image(image) {

        D3D12_DESCRIPTOR_HEAP_DESC descriptor_desc{};
        descriptor_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descriptor_desc.NumDescriptors = 1;

        m_device.get().m_device->CreateDescriptorHeap(&descriptor_desc, IID_PPV_ARGS(&m_ptr));

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	    srv_desc.Format = static_cast<DXGI_FORMAT>(view_format);
	    srv_desc.ViewDimension = static_cast<D3D12_SRV_DIMENSION>(view_type);

        ComponentMapping local_components = components;
        ComponentSwizzleType* swizzles = &local_components.r;

        std::array<uint32, 4> mappings;
        for(uint32 i = 0; i < 4; ++i) {
            if(swizzles[i] == ComponentSwizzleType::Identity) {
                mappings[i] = i;
            } else {
                mappings[i] = static_cast<uint32>(swizzles[i]);
            }
        }
	    srv_desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(mappings[0], mappings[1], mappings[2], mappings[3]);
	    
        m_device.get().m_device->CreateShaderResourceView(image.m_ptr.Get(), &srv_desc, m_ptr->GetCPUDescriptorHandleForHeapStart());
    }

    ImageView(const ImageView&) = delete;

    ImageView(ImageView&& rhs) noexcept : m_device(rhs.m_device), m_image(rhs.m_image) {

    }

    ImageView& operator=(const ImageView&) = delete;

    ImageView& operator=(ImageView&& rhs) noexcept {

        std::swap(m_device, rhs.m_device);
        std::swap(m_image, rhs.m_image);
        return *this;
    }

private:

    std::reference_wrapper<Device> m_device;
    std::reference_wrapper<Image> m_image;

    ComPtr<ID3D12DescriptorHeap> m_ptr;
};

class FramebufferAttachment {
public:

    FramebufferAttachment(Device& device, Image& image) : m_device(device), m_image(image) {

        D3D12_DESCRIPTOR_HEAP_DESC descriptor_desc{};
        descriptor_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descriptor_desc.NumDescriptors = 1;

        m_device.get().m_device->CreateDescriptorHeap(&descriptor_desc, IID_PPV_ARGS(&m_ptr));

        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
        rtv_desc.Format = m_image.value().get().m_desc.Format;
        
        m_device.get().m_device->CreateRenderTargetView(image.m_ptr.Get(), &rtv_desc, m_ptr->GetCPUDescriptorHandleForHeapStart());
    }

    FramebufferAttachment(Device& device, ID3D12Resource* resource) : m_device(device), m_image(std::nullopt) {

        D3D12_DESCRIPTOR_HEAP_DESC descriptor_desc{};
        descriptor_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descriptor_desc.NumDescriptors = 1;

        m_device.get().m_device->CreateDescriptorHeap(&descriptor_desc, IID_PPV_ARGS(&m_ptr));
        m_device.get().m_device->CreateRenderTargetView(resource, nullptr, m_ptr->GetCPUDescriptorHandleForHeapStart());
    }

    FramebufferAttachment(const FramebufferAttachment&) = delete;

    FramebufferAttachment(FramebufferAttachment&& rhs) noexcept : m_device(rhs.m_device), m_image(rhs.m_image) {
        
    }

    FramebufferAttachment& operator=(const FramebufferAttachment&) = delete;

    FramebufferAttachment& operator=(FramebufferAttachment&& rhs) noexcept {

        std::swap(m_device, rhs.m_device);
        std::swap(m_image, rhs.m_image);
        return *this;
    }

private:

    std::reference_wrapper<Device> m_device;
    std::optional<std::reference_wrapper<Image>> m_image;

    ComPtr<ID3D12DescriptorHeap> m_ptr;
};

}