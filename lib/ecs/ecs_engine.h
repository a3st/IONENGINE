// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::ecs {

class Engine final {
friend class EntityManager;
friend class ComponentManager;
public:

    Engine() {
        ecs::ComponentManager::get_instance();
        ecs::EntityManager::get_instance();
    }

private:

    
};

}