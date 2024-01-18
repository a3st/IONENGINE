// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "primitive_cache.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

PrimitiveCache::PrimitiveCache(
    Context& context, 
    BufferAllocator<LinearAllocator>& allocator
) : 
    context(&context), 
    allocator(&allocator) 
{

}

auto PrimitiveCache::get(PrimitiveData const& data) -> core::ref_ptr<Primitive> {

    auto entry = entries.find(data.hash);

    if(entry != entries.end()) {
        if(!std::get<0>(entry->second)) {
            auto primitive = core::make_ref<Primitive>(*context, *allocator, data);
            entries.emplace(data.hash, std::make_tuple(primitive, 60));
        }
        return std::get<0>(entry->second);
    } else {
        auto primitive = core::make_ref<Primitive>(*context, *allocator, data);
        entries.emplace(data.hash, std::make_tuple(primitive, 60));
        return primitive;
    }
}

auto PrimitiveCache::update(float const dt) -> void {

    for(auto& entry : entries) {
        if(std::get<0>(entry.second)) {
            if(std::get<1>(entry.second) > 0) {
                std::get<1>(entry.second) -= dt;
            } else {
                std::get<0>(entry.second) = nullptr;
            }
        }
    }
}