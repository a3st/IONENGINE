// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class VKSampler : public Sampler {
public:

    VKSampler() {

    }

    const SamplerDesc& get_desc() const override { return m_desc; }

private:

    SamplerDesc m_desc;
};

}
