// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../singleton.h"
#include "../sparse_set.h"

namespace ionengine::ecs {

class ComponentManager final : public Singleton<ComponentManager> {
DECLARE_SINGLETON(ComponentManager)
public:

    

protected:

    ComponentManager() {
    }

private:

    std::unordered_map<
        uint32,
        sparse_set<uint32>
    > m_components;

};

}