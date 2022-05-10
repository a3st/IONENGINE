// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

template<class Type>
struct CacheEntry {
    Type value;
    uint64_t hash;
};

template<class Type>
class CachePool { };

}
