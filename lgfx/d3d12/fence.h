// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class Fence {

friend class Device;

public:

    Fence();
    ~Fence();
    Fence(Device* device, const uint64_t initial_value);
    Fence(const Fence&) = delete;
    Fence(Fence&& rhs) noexcept;
    
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&& rhs) noexcept;

    uint64_t GetCompletedValue() const;
    void Signal(const uint64_t value);
    void Wait(const uint64_t value);

private:

    ID3D12Fence* fence_;
    HANDLE event_;
};

}