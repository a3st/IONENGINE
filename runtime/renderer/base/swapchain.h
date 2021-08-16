// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Swapchain {
public:

    virtual ~Swapchain() = default;
    
    virtual Resource& get_back_buffer(const uint32 buffer_index) = 0;
    virtual void present(Fence& fence, const uint64 wait_value) = 0;
    virtual uint32 next_buffer(Fence& fence, const uint64 signal_value) = 0;
    virtual uint32 get_back_buffer_index() const = 0;
};

}