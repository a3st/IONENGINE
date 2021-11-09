// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "fence.h"
#include "device.h"

using namespace lgfx;

Fence::Fence(Device* const device, const uint64_t initial_value) {

    assert(device && "invalid pointer to device");

    THROW_IF_FAILED(device->device_->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(fence_.GetAddressOf())));

    event_ = UniqueHANDLE(CreateEvent(nullptr, false, false, nullptr));
    if(!event_) {
        THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
    }
}

uint64_t Fence::GetCompletedValue() const {

    return fence_->GetCompletedValue();
}

void Fence::Signal(const uint64_t value) {

    THROW_IF_FAILED(fence_->Signal(value));
}

void Fence::Wait(const uint64_t value) {

    if(fence_->GetCompletedValue() < value) {
        THROW_IF_FAILED(fence_->SetEventOnCompletion(value, reinterpret_cast<HANDLE>(event_.get())));
        WaitForSingleObjectEx(reinterpret_cast<HANDLE>(event_.get()), INFINITE, false);
    }
}