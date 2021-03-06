// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_event.h>
#include <asset/asset_ptr.h>
#include <lib/event_dispatcher.h>
#include <lib/logger.h>

namespace ionengine::asset {

template<class Type>
class AssetLoader {
public:

    AssetLoader() = default;

    void load_asset(lib::Logger& logger, AssetPtr<Type> asset, lib::EventDispatcher<AssetEvent<Type>>& event_dispatcher);
};

}