// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class Sampler {
public:

    virtual ~Sampler() = default;

    virtual const SamplerDesc& get_desc() const = 0;
};

}