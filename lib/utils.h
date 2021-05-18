// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<typename T>
struct type { 
    static void id() { }
};

template<typename T>
usize type_id() { return reinterpret_cast<usize>(&type<T>::id); }

}