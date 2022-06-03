// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/subscene.h>

namespace ionengine::scene {

template<class Type>
struct CacheEntry {
    Type value;
    uint64_t hash;
};

class StreamingSceneCache {
public:

private:

};

}