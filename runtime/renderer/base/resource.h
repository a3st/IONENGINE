// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Resource {
public:

    virtual ~Resource() = default;
    virtual void bind_memory(const std::shared_ptr<Memory>& memory, const uint64 offset) = 0; 
};

}