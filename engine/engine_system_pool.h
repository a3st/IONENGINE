// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine_system.h"

namespace ionengine {
    
class EngineSystemPool {
public:

    EngineSystemPool() {

    }

    template<class T, typename... A>
    T* initialize_system(A&&... args) {

        m_engine_systems.emplace_back(std::make_unique<T>(std::forward<A>(args)...));
        return static_cast<T*>((*std::prev(m_engine_systems.end())).get());
    }

    void execute() {
        for(auto& engine_system : m_engine_systems) {
            engine_system->tick();
        }
    }
    
private:

    std::list<std::unique_ptr<EngineSystem>> m_engine_systems;
};

}