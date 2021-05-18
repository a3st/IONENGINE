// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../singleton.h"
#include "../sparse_set.h"
#include "../utils.h"

namespace ionengine::ecs {

class ComponentManager final : public Singleton<ComponentManager> {
DECLARE_SINGLETON(ComponentManager)
public:

    template<typename T>
    void register_component() {
        component_id id = static_cast<component_id>(type_id<T>());
        assert(m_component_arrays.find(id) == m_component_arrays.end() && "error during registering components");
        m_component_arrays[id] = component_array<T>();
    }

protected:

    ComponentManager() {
    }

private:

    std::unordered_map<
        component_id,
        basic_component_array
    > m_component_arrays;

};

}