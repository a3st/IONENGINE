// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief Fence (backend::base)

    Fence base template class
*/

template<class B = backend::base>
class Fence {
public:

    uint64 get_completed_value() const;
};

template<class B = backend::base>
void wait_fence(Fence<B>* fence, const uint64 value);

template<class B = backend::base>
void signal_fence(Fence<B>* fence, const uint64 value);

}