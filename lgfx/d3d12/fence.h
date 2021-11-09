// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "handle_ptr.h"

namespace lgfx {

class Fence {

friend class Device;

public:

    Fence() = default;

    Fence(Device* const device, const uint64_t initial_value);

    Fence(const Fence&) = delete;

    Fence(Fence&& rhs) noexcept = default;

    Fence& operator=(const Fence&) = delete;

    Fence& operator=(Fence&& rhs) noexcept = default;

    uint64_t GetCompletedValue() const;
    void Signal(const uint64_t value);
    void Wait(const uint64_t value);

private:

    ComPtr<ID3D12Fence> fence_;
    UniqueHANDLE event_;
};

}