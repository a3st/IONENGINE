// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief Device (backend::base)

    Device base template class
*/

template<class B = backend::base>
class Device { };

template<class B = backend::base>
const AdapterDesc& get_adapter_desc(Device<B>* device);

template<class B = backend::base>
uint32 get_swapchain_buffer_index(Device<B>* device);

template<class B = backend::base>
uint32 get_swapchain_buffer_size(Device<B>* device);

}