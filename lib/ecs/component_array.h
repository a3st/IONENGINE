// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::ecs {

class basic_component_array {

};

template<typename T>
class component_array : public basic_component_array {
public:

    component_array() {
    }

private:

    std::array<T, max_entities> m_components;
};

}

