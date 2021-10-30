// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

const size_t kDescriptorSetSRVCount = 64;
const size_t kDescriptorSetSamplerCount = 16;

class DescriptorSet {

public:

    DescriptorSet(Device* device);

    void WriteTexture(const uint32_t slot, const uint32_t space, TextureView* texture_view);
    void WriteBuffer(const uint32_t slot, const uint32_t space, BufferView* buffer_view);

private:

    lgfx::DescriptorPool srv_pool;
    lgfx::DescriptorPool sampler_pool;

};

}