// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Fence {
public:

    virtual ~Fence() = default;
    
    virtual uint64 get_completed_value() = 0;
    virtual void wait(const uint64 value) = 0;
    virtual void signal(const uint64 value) = 0;
};

}