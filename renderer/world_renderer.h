// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>

namespace ionengine::renderer {

class WorldRenderer {
public:

    WorldRenderer(Backend* const backend);

    WorldRenderer(WorldRenderer const&) = delete;

    WorldRenderer(WorldRenderer&&) = delete;

    WorldRenderer& operator=(WorldRenderer const&) = delete;

    WorldRenderer& operator=(WorldRenderer&&) = delete;

    void update();

private:

    Backend* backend_;

};

}