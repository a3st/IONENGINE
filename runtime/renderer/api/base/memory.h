// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class Memory {
public:

    virtual ~Memory() = default;
    virtual MemoryType get_type() const = 0;
};


}