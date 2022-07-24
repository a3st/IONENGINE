// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core {

class Allocator {
 public:
    virtual ~Allocator() = default;

    virtual void* allocate(size_t const size) = 0;

    virtual void deallocate(void* other) = 0;
};

}  // namespace ionengine::core