// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../singleton.h"

namespace ionengine::ecs {

class EntityManager final : public Singleton<EntityManager> {
DECLARE_SINGLETON(EntityManager)
public:

    void create_layer() {

    }

protected:

    EntityManager() {
    }

private:

    

};

}