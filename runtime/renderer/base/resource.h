// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Resource {
public:

    virtual ~Resource() = default;
    virtual void bind_memory(Memory& memory, const uint64 offset) = 0;
    virtual const std::variant<ResourceDesc, SamplerDesc>& get_desc() const = 0;
};

}