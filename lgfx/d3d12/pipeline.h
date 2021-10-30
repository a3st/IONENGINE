// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

class Pipeline {

friend class CommandBuffer;

public:

    Pipeline(Device* device, const PipelineDesc& desc);

    inline const PipelineDesc& GetDesc() const { return desc_; }

private:

    ComPtr<ID3D12PipelineState> pipeline_state_;

    PipelineDesc desc_;
};

}