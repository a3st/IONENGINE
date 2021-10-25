// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core {

template<class T>
class ObjectPool {

public:

    ObjectPool(const size_t size) : pool_size_(size)  {

    }

    

private:

    std::vector<T> objects_;
    std::vector<uint8_t> blocks_;

    size_t pool_size_;

};

}