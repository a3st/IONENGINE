// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/context.h>
#include <renderer/geometry.h>

namespace ionengine::renderer {

template<class Type>
struct CacheEntry {
    Type value;
    uint64_t value_hash;
};

class GeometryCache {
public:

    GeometryCache();

    GeometryBuffer& get_from(Context& context, asset::Mesh& mesh);

private:

    std::vector<CacheEntry<GeometryBuffer>> _cache;

};

}