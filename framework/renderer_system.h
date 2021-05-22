// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"
#include "renderer/base_renderer.h"
#include "platform/window.h"

namespace ionengine::renderer {

class RenderSystem final : public Singleton<RenderSystem> {
DECLARE_SINGLETON(RenderSystem)
public:

    template<typename T = BaseRenderer>
    void create_renderer(platform::Window& window) {
        
    }

protected:

    RenderSystem() {
        
    }

private:


};

}