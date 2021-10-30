// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class DescriptorLayout {

friend class Pipeline;

public:

    DescriptorLayout(Device* device, const std::span<DescriptorLayoutBinding>& bindings);

private:

    ComPtr<ID3D12RootSignature> root_signature_;

};

}