// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

/**
    @brief Graphics API Instance class

    Instance class for Graphics API. It's needed for enumerate adapter
*/
class Instance {
public:

    virtual ~Instance() = default;

    /**
        @brief Enumerate adapters
        @return std::vector<std::unique_ptr<Adapter>> vector of unique pointers to adapters
    */
    virtual std::vector<std::unique_ptr<Adapter>> enumerate_adapters() = 0;
};


}