// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

const size_t kDescriptorSetSRVCount = 64;
const size_t kDescriptorSetSamplerCount = 16;

class DescriptorSet {

friend class CommandBuffer;

public:

    DescriptorSet(Device* const device, DescriptorLayout* const layout);
    ~DescriptorSet();
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&& rhs) noexcept;

    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&& rhs) noexcept;

    void WriteTexture(const uint32_t slot, const uint32_t space, TextureView* texture_view);
    void WriteBuffer(const uint32_t slot, const uint32_t space, BufferView* buffer_view);

private:

    struct Key {
        
        uint32_t index;
        DescriptorType type;
        DescriptorAllocInfo alloc_info;
    };

    Device* device_;
    DescriptorLayout* layout_;

    DescriptorPool srv_pool_;
    DescriptorPool sampler_pool_;

    std::vector<Key> update_descriptors_;

    std::map<uint32_t, std::vector<Key>> descriptors_;
};

}