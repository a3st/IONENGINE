// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

struct DescriptorTable {

    DescriptorType type;
    uint32_t count;
    uint32_t slot;
};

class DescriptorLayout {

friend class Pipeline;
friend class DescriptorSet;
friend class CommandBuffer;

public:

    DescriptorLayout(Device* device, const std::span<DescriptorLayoutBinding>& bindings);

private:

    ComPtr<ID3D12RootSignature> root_signature_;
    std::vector<DescriptorTable> descriptor_tables_;
};

}