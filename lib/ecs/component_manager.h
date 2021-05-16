// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../singleton.h"

namespace ionengine::ecs {

class ComponentManager final : public Singleton<ComponentManager> {
DECLARE_SINGLETON(ComponentManager)
public:

    void test() const {
        std::cout << 123 << std::endl;
    }

protected:

    ComponentManager() {
    }

private:

    

};

}