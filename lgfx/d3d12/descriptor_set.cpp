// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "descriptor_set.h"
#include "texture_view.h"
#include "buffer_view.h"

using namespace lgfx;

DescriptorSet::DescriptorSet(Device* device) : 
    srv_pool(device, kDescriptorSetSRVCount, lgfx::DescriptorType::kShaderResource, lgfx::DescriptorFlags::kShaderVisible),
    sampler_pool(device, kDescriptorSetSamplerCount, lgfx::DescriptorType::kSampler, lgfx::DescriptorFlags::kShaderVisible) {

    
}

void DescriptorSet::WriteTexture(const uint32_t slot, const uint32_t space, TextureView* texture_view) {

    
}

void DescriptorSet::WriteBuffer(const uint32_t slot, const uint32_t space, BufferView* buffer_view) {


}