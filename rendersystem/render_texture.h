// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rendersystem {

class RenderTexture {
public:

    enum class Alias : uint64 {
        Swapchain_0 = 0,
        Swapchain_1 = 1,
        Swapchain_2 = 2,
        Swapchain_3 = 3,
        Swapchain_4 = 4,
        Swapchain_5 = 5,

        RenderTarget_0 = 10,
        RenderTarget_1 = 11,
        RenderTarget_2 = 12,
        RenderTarget_3 = 13,
        RenderTarget_4 = 14,
        RenderTarget_5 = 15,
        RenderTarget_6 = 16,
        RenderTarget_7 = 17,
        RenderTarget_8 = 18,
        RenderTarget_9 = 19,
        RenderTarget_10 = 20
    };

    enum class Format {
        Unknown
    };

    RenderTexture(gfx::Device* device, const uint64 id) : m_device(device), m_id(id) {   

    }

    [[nodiscard]] lib::Expected<RenderTexture*, std::string> create_from_swapchain(const uint32 buffer_index) {

        if(buffer_index + 1 > m_device->get_swapchain_buffer_size()) {
            return lib::make_expected<RenderTexture*, std::string>("<RenderTexture> Buffer size value is bigger than swapchain has");
        }

        m_resource = m_device->create_swapchain_resource(buffer_index);

        auto& resource_desc = m_resource->get_desc();
        m_width = static_cast<uint32>(resource_desc.width);
        m_height = resource_desc.height;

        gfx::ViewDesc view_desc{};
        view_desc.dimension = gfx::ViewDimension::Texture2D;

        m_view = m_device->create_view(gfx::ViewType::RenderTarget, m_resource.get(), view_desc);

        return lib::make_expected<RenderTexture*, std::string>(this);
    }

    uint32 get_width() const { return m_width; }
    
    uint32 get_height() const { return m_height; }

    gfx::View* get_view() const { return m_view.get(); }

    gfx::Resource* get_resource() const { return m_resource.get(); }

private:

    const uint64 m_id;

    gfx::Device* m_device;

    std::unique_ptr<gfx::View> m_view;
    std::unique_ptr<gfx::Resource> m_resource;

    uint32 m_width;
    uint32 m_height;
};

}