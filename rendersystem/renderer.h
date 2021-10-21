// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"
#include "../platform/platform.h"

namespace ionengine::rendersystem {

class Renderer {

public:

    Renderer(platform::Window* window);

    void Frame();

private:

    platform::Window* window_;

    lgfx::Device device_;
};

}