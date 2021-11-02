// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

const uint32_t kDescriptorPoolDefaultHeapSize = 64;

class DescriptorHeap {

friend class DescriptorPool;
friend class TextureView;
friend class BufferView;
friend class CommandBuffer;

public:

    DescriptorHeap(Device* device, const DescriptorType type, const DescriptorFlags flags);
    DescriptorHeap(const DescriptorHeap&) = delete;
    DescriptorHeap(DescriptorHeap&&) = delete;

    DescriptorHeap& operator=(const DescriptorHeap&) = delete;
    DescriptorHeap& operator=(DescriptorHeap&&) = delete;

    inline size_t GetHeapSize() const { return heap_size_; }

private:

    ComPtr<ID3D12DescriptorHeap> heap_;
    size_t heap_size_;
    uint64_t offset_;
    std::vector<uint8_t> descriptors_;
};

struct DescriptorAllocInfo {

    DescriptorHeap* heap;
    uint32_t offset;
};

class DescriptorPool {

public:

    DescriptorPool(Device* device, const size_t size, const DescriptorType type, const DescriptorFlags flags);
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&&) = delete;

    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&) = delete;

    inline DescriptorType GetType() const { return type_; }
    inline DescriptorFlags GetFlags() const { return flags_; }

    DescriptorAllocInfo Allocate();
    void Deallocate(const DescriptorAllocInfo& alloc_info);

private:

    DescriptorType type_;
    DescriptorFlags flags_;
    std::vector<std::unique_ptr<DescriptorHeap>> heaps_;
};

}