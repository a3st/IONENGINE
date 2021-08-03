// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Instance {
public:

    virtual ~Instance() = default;
    virtual std::vector<std::unique_ptr<Adapter>> enumerate_adapters() = 0;
};


}