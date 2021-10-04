// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief GAPI Wrapper Fence class

    Fence is primitive synchronization data between GPU and CPU
*/

template<class B = backend::base>
class Fence {
public:
    
    uint64 get_completed_value() const;

    void wait(const uint64 value);

    void signal(const uint64 value);
};

}