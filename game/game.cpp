// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "game.hpp"
#include "precompiled.h"
#include "engine/scene/sprite_node.hpp"

using namespace ionengine;

Game::Game(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options)
{
}

Game::Game(core::ref_ptr<platform::App> app, EngineEnvironment& environment)
{
}

auto Game::initialize() -> void
{
    auto backgroundSpriteNode = core::make_ref<SpriteNode>("Background");
    
}

auto Game::shutdown() -> void
{
}