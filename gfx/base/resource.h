// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class Resource {
public:

    virtual ~Resource() = default;

    virtual ResourceType get_type() const = 0;

    virtual const std::variant<ResourceDesc, SamplerDesc>& get_desc() const = 0;

    virtual byte* map() = 0;

    virtual void unmap() = 0;
};

}