// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/device.hpp"

namespace ionengine {

namespace renderer {

struct PrimitiveData {
    std::span<uint8_t const> vertices;
    std::span<uint8_t const> indices;
    uint64_t hash;
};

using Primitive = std::pair<core::ref_ptr<rhi::Buffer>, core::ref_ptr<rhi::Buffer>>;

class PrimitiveCache {
public:

    PrimitiveCache(rhi::Device& device);

    auto get(PrimitiveData const& data, bool const immediate = false) -> std::optional<Primitive>;

    auto update(float const dt) -> void;

private:

    inline static uint32_t ALLOCATE_SIZE_PER_FRAME = 8 * 1024 * 1024;
    size_t size{0};

    rhi::Device* device;
    core::ref_ptr<rhi::MemoryAllocator> allocator{nullptr};

    struct PrimitiveInfo {
        rhi::Future<rhi::Buffer> vertices;
        rhi::Future<rhi::Buffer> indices;
        uint64_t lifetime;
    };
    std::unordered_map<uint64_t, PrimitiveInfo> entries;
};

}

}