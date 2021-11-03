
#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class Sampler {

public:

    Sampler(Device* device);
    Sampler(const Sampler&) = delete;
    Sampler(Sampler&&) = delete;

    Sampler& operator=(const Sampler&) = delete;


private:

    D3D12_SAMPLER_DESC sampler_;

};

}