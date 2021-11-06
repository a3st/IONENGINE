// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

const uint32_t kDescriptorPoolDefaultHeapSize = 8;

class DescriptorHeap {

friend class DescriptorPool;
friend class TextureView;
friend class BufferView;
friend class CommandBuffer;
friend class DescriptorSet;

public:

    DescriptorHeap(Device* const device, const DescriptorType type, const DescriptorFlags flags);
    DescriptorHeap(const DescriptorHeap&) = delete;
    DescriptorHeap(DescriptorHeap&& rhs) noexcept;

    DescriptorHeap& operator=(const DescriptorHeap&) = delete;
    DescriptorHeap& operator=(DescriptorHeap&& rhs) noexcept;

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

    DescriptorPool();
    DescriptorPool(Device* const device, const size_t size, const DescriptorType type, const DescriptorFlags flags);
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&& rhs) noexcept;

    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&& rhs) noexcept;

    inline DescriptorType GetType() const { return type_; }
    inline DescriptorFlags GetFlags() const { return flags_; }
    inline std::span<const DescriptorHeap> GetHeaps() const { return heaps_; }

    DescriptorAllocInfo Allocate();
    void Deallocate(const DescriptorAllocInfo& alloc_info);

private:

    DescriptorType type_;
    DescriptorFlags flags_;
    std::vector<DescriptorHeap> heaps_;
};

}