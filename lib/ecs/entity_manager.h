// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../singleton.h"
#include "../sparse_set.h"

namespace ionengine::ecs {

class EntityManager final : public Singleton<EntityManager> {
DECLARE_SINGLETON(EntityManager)
public:

    uint64 create_entity(const std::string& name) {
        
        uint64 entity_id = m_entities.size();
        m_proxy_names[name] = entity_id;
        m_entities.insert(static_cast<uint64>(entity_id));
        return entity_id;
    }

protected:

    EntityManager() {
    }

private:

    sparse_set<uint64> m_entities;
    std::unordered_map<std::string, uint32> m_proxy_names;

};

}