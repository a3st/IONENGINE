// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "primitive.hpp"

namespace ionengine {

namespace renderer {

class Context;

class PrimitiveCache {
public:

    PrimitiveCache(Context& context, BufferAllocator<LinearAllocator>& allocator);

    auto get(PrimitiveData const& data) -> core::ref_ptr<Primitive>;

    auto update(float const dt) -> void;

private:

    Context* context;
    BufferAllocator<LinearAllocator>* allocator;
    std::unordered_map<uint64_t, std::tuple<core::ref_ptr<Primitive>, uint32_t>> entries;
};

}

}