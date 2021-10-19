// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

const uint32_t kDescriptorPoolDefaultHeapSize = 64;

struct DescriptorHeap {
    
    ComPtr<ID3D12DescriptorHeap> heap;

    size_t heap_size;

    uint64_t offset;

    std::vector<uint8_t> descriptors;

    DescriptorHeap();
    DescriptorHeap(Device* device, const DescriptorType type, const DescriptorFlags flags);
    DescriptorHeap(const DescriptorHeap&) = delete;
    DescriptorHeap(DescriptorHeap&& rhs) noexcept;

    DescriptorHeap& operator=(const DescriptorHeap&) = delete;
    DescriptorHeap& operator=(DescriptorHeap&& rhs) noexcept;
};

struct DescriptorPtr {
    DescriptorHeap* heap;
    uint32_t offset;
};

class DescriptorPool {

public:

    DescriptorPool();
    DescriptorPool(Device* device, const size_t size, const DescriptorType type, const DescriptorFlags flags);
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&& rhs) noexcept;

    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&& rhs) noexcept;

    inline DescriptorType GetType() const { return type_; }
    inline DescriptorFlags GetFlags() const { return flags_; }

    DescriptorPtr Allocate();
    void Deallocate(DescriptorPtr* ptr);

private:

    DescriptorType type_;
    DescriptorFlags flags_;

    std::vector<DescriptorHeap> heaps_;
};

}