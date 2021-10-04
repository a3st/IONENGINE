// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<class B = backend::base>
class Resource {
public:

    ResourceType get_type() const;

    const std::variant<SamplerDesc, ResourceDesc>& get_desc() const;

    byte* map();

    void unmap();
};

}