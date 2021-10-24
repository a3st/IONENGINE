// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

const uint32_t kDescriptorPoolDefaultHeapSize = 64;

class DescriptorHeap {

friend class DescriptorPool;
friend class TextureView;
friend class CommandBuffer;

public:

    DescriptorHeap(Device* device, const DescriptorType type, const DescriptorFlags flags);

private:

    ComPtr<ID3D12DescriptorHeap> heap_;

    size_t heap_size_;

    uint64_t offset_;

    std::vector<uint8_t> descriptors_;
};

struct DescriptorPtr {

    DescriptorHeap* heap;
    uint32_t offset;

    inline bool operator!() const {
        
        return !heap;
    }
};

class DescriptorPool {

public:

    DescriptorPool(Device* device, const size_t size, const DescriptorType type, const DescriptorFlags flags);

    inline DescriptorType GetType() const { return type_; }
    inline DescriptorFlags GetFlags() const { return flags_; }

    DescriptorPtr Allocate();
    void Deallocate(DescriptorPtr* ptr);

private:

    DescriptorType type_;
    DescriptorFlags flags_;

    std::vector<std::unique_ptr<DescriptorHeap>> heaps_;
};

}