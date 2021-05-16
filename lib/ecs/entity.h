// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::ecs {

class Entity {
public:

    Entity();

private:

    uint64 m_id;
    std::wstring m_name;
};

}