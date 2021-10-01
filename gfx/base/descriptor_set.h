// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class DescriptorSet {
public:

    virtual ~DescriptorSet() = default;

    virtual void update_descriptor(const uint32 slot, View* view) = 0;

    virtual void copy_descriptor(const uint32 src_slot, DescriptorSet* dest, const uint32 dst_slot) = 0;
};

}