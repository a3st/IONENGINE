// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../singleton.h"
#include "../sparse_set.h"

namespace ionengine::ecs {

class EntityManager final : public Singleton<EntityManager> {
DECLARE_SINGLETON(EntityManager)
public:

    entity_id create_entity(const std::string& name) {
        return 0;
    }

protected:

    EntityManager() : m_size(0) {
    }

private:

   
    usize m_size;

};

}