// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Image final {
public:

    Image(Device& device, const ImageType image_type, const uint32 width, const uint32 height, const ImageFormat image_format) : m_device(device) {

        vk::ImageCreateInfo image_info{};

        image_info
            .setFormat(static_cast<vk::Format>(image_format))
            .setImageType(static_cast<vk::ImageType>(image_type))
            .setExtent({ width, height, 1 })
            .setMipLevels(1)
            .setArrayLayers(1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setSharingMode(vk::SharingMode::eExclusive);

        m_handle = m_device.get().get_handle()->createImageUnique(image_info);
    }

    Image(const Image&) = delete;

    Image(Image&& rhs) noexcept : m_device(rhs.m_device) {

        m_handle.swap(rhs.m_handle);
    }

    Image& operator=(const Image&) = delete;

    Image& operator=(Image&& rhs) noexcept {

        std::swap(m_device, rhs.m_device);

        m_handle.swap(rhs.m_handle);
        return *this;
    }

    const vk::UniqueImage& get_handle() const { return m_handle; }

private:

    std::reference_wrapper<Device> m_device;

    vk::UniqueImage m_handle;
};

class ImageView final {
public:

    ImageView(Device& device, Image& image, const ImageViewType view_type, const ImageFormat image_format) : m_device(device) {

        vk::ImageViewCreateInfo view_info{};

        view_info
            .setImage(image.get_handle().get())
            .setViewType(static_cast<vk::ImageViewType>(view_type))
            .setFormat(static_cast<vk::Format>(image_format))
            .setComponents({vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity})
            .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        m_handle = m_device.get().get_handle()->createImageViewUnique(view_info);
    }

    ImageView(const ImageView&) = delete;

    ImageView(ImageView&& rhs) noexcept : m_device(rhs.m_device) {

        m_handle.swap(rhs.m_handle);
    }

    ImageView& operator=(const ImageView&) = delete;

    ImageView& operator=(ImageView&& rhs) noexcept {

        std::swap(m_device, rhs.m_device);

        m_handle.swap(rhs.m_handle);
        return *this;
    }

    const vk::UniqueImageView& get_handle() { return m_handle; }

private:

    std::reference_wrapper<Device> m_device;

    vk::UniqueImageView m_handle;
};

}