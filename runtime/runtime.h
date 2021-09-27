// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

namespace renderer {
class Renderer;
}

namespace framework {
class AppFramework;
}

struct EngineEnv {
    renderer::Renderer* renderer;
    framework::AppFramework* app_framework;
};

extern std::unique_ptr<EngineEnv> g_env;

}