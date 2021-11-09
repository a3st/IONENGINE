
#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class Sampler {

public:

    Sampler() = default;

    Sampler(Device* device);
    
    Sampler(const Sampler&) = delete;

    Sampler(Sampler&&) noexcept = default;

    Sampler& operator=(const Sampler&) = delete;
    
    Sampler& operator=(Sampler&& rhs) noexcept = default;

private:

    D3D12_SAMPLER_DESC sampler_;

};

}