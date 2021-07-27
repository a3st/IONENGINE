// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class ImageType {
    Single1D = VK_IMAGE_TYPE_1D,
    Single2D = VK_IMAGE_TYPE_2D, 
    Single3D = VK_IMAGE_TYPE_3D
};

class Image final {
public:

    Image(const Device& device, const ImageType image_type, const uint32 width, const uint32 height, const ImageFormat image_format) :
        m_device(device) {

        VkImageCreateInfo image_info = {};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        
        image_info.extent.width = width;
        image_info.extent.height = height;
        image_info.extent.depth = 1;
        
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = static_cast<VkFormat>(image_format);
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.imageType = static_cast<VkImageType>(image_type);
        
        throw_if_failed(vkCreateImage(device.get_handle(), &image_info, nullptr, &m_image_handle));
    }
    
    Image(const Device& device, VkImage rhs) : m_device(device), m_image_handle(rhs) {
        
    }

    ~Image() {
       // vkDestroyImage(m_device.get_handle(), m_image_handle, nullptr);
    }

    const VkImage& get_handle() const { return m_image_handle; }

private:

    const Device& m_device;

    VkImage m_image_handle;
};

}