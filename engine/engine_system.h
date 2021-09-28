// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {
    
class EngineSystem {
public:

    virtual ~EngineSystem() = default;

    virtual void tick() = 0;
};

}