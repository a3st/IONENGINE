// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/vector2.h"

namespace ionengine::rendersystem {

class Texture {
public:

    enum class Type {
        _2D,
        Cube
    };

    enum class Usage {
        Default,
        RenderTarget,
        Swapchain,
        DepthStencil
    };

    enum class Format {
        RGBA8,
        DXT1,
        DXT5,
        D32
    };

    Texture(gfx::Device* device) 
        : 
            m_device(device),
            m_size{},
            m_usage(Texture::Usage::Default) {   

            assert(device && "pointer to device is null");
    }



    [[nodiscard]] lib::Expected<Texture*, std::string> create_from_swapchain(const uint32 buffer_index) {

        if(buffer_index + 1 > m_device->get_swapchain_buffer_size()) {
            return lib::make_expected<Texture*, std::string>("<Texture> Buffer size value is bigger than swapchain has");
        }

        m_resource = m_device->create_swapchain_resource(buffer_index);

        auto& resource_desc = std::get<gfx::ResourceDesc>(m_resource->get_desc());
        m_size.x = static_cast<uint32>(resource_desc.width);
        m_size.y = resource_desc.height;

        m_usage = Texture::Usage::Swapchain;
        m_format = Texture::Format::RGBA8;

        gfx::ViewDesc view_desc{};
        view_desc.dimension = gfx::ViewDimension::Texture2D;

        m_view = m_device->create_view(gfx::ViewType::RenderTarget, m_resource.get(), view_desc);

        return lib::make_expected<Texture*, std::string>(this);
    }

    const math::Uvector2& get_size() const { return m_size; }

    gfx::View* get_view() const { return m_view.get(); }

    gfx::Resource* get_resource() const { return m_resource.get(); }

    Texture::Usage get_usage() const { return m_usage; }

private:

    gfx::Device* m_device;

    std::unique_ptr<gfx::View> m_view;
    std::unique_ptr<gfx::Resource> m_resource;

    math::Uvector2 m_size;

    Texture::Usage m_usage;
    Texture::Format m_format;
};

}

namespace std {

using namespace ionengine;

template<> struct hash<rendersystem::Texture> {
    
    usize operator()(const rendersystem::Texture& rhs) const noexcept {

        usize h2 = std::hash<uint32>{}(rhs.get_size().x);
        usize h3 = std::hash<uint32>{}(rhs.get_size().y);
        usize h4 = std::hash<uint32>{}(static_cast<uint32>(rhs.get_usage()));
        return h2 ^ h3 ^ h4;
    }
};

}