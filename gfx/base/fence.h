// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief GAPI Wrapper Fence class

    Fence is primitive synchronization data between GPU and CPU
*/
class Fence {
public:

    virtual ~Fence() = default;
    
    /**
        @brief Get a completed value
        @return uint64 Completed value
    */
    virtual uint64 get_completed_value() const = 0;

    /**
        @brief Wait for value fence
        @param value Wait value
    */
    virtual void wait(const uint64 value) = 0;

    /**
        @brief Signal for value fence
        @param value Signal value
    */
    virtual void signal(const uint64 value) = 0;
};

}