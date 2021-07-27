// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {
    
enum class ImageViewType {
    Single1D = VK_IMAGE_VIEW_TYPE_1D,
    Single2D = VK_IMAGE_VIEW_TYPE_2D,
    Single3D = VK_IMAGE_VIEW_TYPE_3D,
    SingleCube = VK_IMAGE_VIEW_TYPE_CUBE,
    Array1D = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
    Array2D = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
    ArrayCube = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
};

class ImageView final {
public:

    ImageView(const Device& device, const Image& image, const ImageViewType view_type, const ImageFormat image_format) : m_device(device) {

        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image.get_handle();
        view_info.viewType = static_cast<VkImageViewType>(view_type);
        view_info.format = static_cast<VkFormat>(image_format);
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        throw_if_failed(vkCreateImageView(m_device.get_handle(), &view_info, nullptr, &m_image_view_handle));
    }

    ~ImageView() {
        vkDestroyImageView(m_device.get_handle(), m_image_view_handle, nullptr);
    }

    const VkImageView& get_handle() { return m_image_view_handle; }

private:

    const Device& m_device;

    VkImageView m_image_view_handle;
};

}