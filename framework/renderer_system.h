// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"
#include "renderer/renderer.h"
#include "platform/window.h"

namespace ionengine::renderer {

class RenderSystem final : public Singleton<RenderSystem> {
DECLARE_SINGLETON(RenderSystem)
public:

    template<typename T = Renderer>
    void create_renderer_for(platform::Window& window) {

    }

protected:

    RenderSystem() {
        
    }

private:
};

}