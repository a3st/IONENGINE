// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#define ENTITY_NULL UINT64_MAX

namespace ionengine::ecs {

typedef uint64 entity_id;
typedef uint8 component_id;

constexpr uint32 max_entities = 5000;
constexpr uint32 max_components = 50;

}

#include "ecs/system.h"
#include "ecs/entity_manager.h"
#include "ecs/component_array.h"
#include "ecs/component_manager.h"
#include "ecs/ecs_engine.h"