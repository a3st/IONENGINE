// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "framegraph_resource.h"

namespace ionengine::rendersystem {

struct FrameGraphResourceDesc {
    bool acquired;
    uint32 frame_index;
    std::unique_ptr<FrameGraphResource> resource;
};

class FrameGraphResourcePool {
public:

    FrameGraphResourcePool(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");
    }

    FrameGraphResource* allocate(
        const FrameGraphResourceType resource_type,
        Texture* texture,
        const FrameGraphResourceFlags resource_flags
    ) {

        FrameGraphResource* ptr = nullptr;

        for(auto& res : m_resources) {

            if(!res.acquired && res.frame_index == m_device->get_swapchain_buffer_index() &&
                res.resource->get_texture() == texture && res.resource->get_flags() == resource_flags) {

                ptr = res.resource.get();
                //std::cout << "found frame graph resource free" << std::endl;
                break;
            }
        }

        if(!ptr) {

            m_resources.emplace_back(
                FrameGraphResourceDesc {
                    true,
                    m_device->get_swapchain_buffer_index(),
                    std::make_unique<FrameGraphResource>(texture)
                }
            );

            ptr = m_resources.back().resource.get();

            //std::cout << "allocating new frame graph resource" << std::endl;
        }
        return ptr;
    }

    void deallocate(FrameGraphResource* resource) {

        for(auto& res : m_resources) {
            if(res.resource.get() == resource) {
                res.acquired = false;
                //std::cout << "frame graph resource deallocated" << std::endl;
                break;
            }
        }
    }

    void clear() {
        
        m_resources.clear();
    }

private:

    gfx::Device* m_device;

    std::list<FrameGraphResourceDesc> m_resources;
};

}