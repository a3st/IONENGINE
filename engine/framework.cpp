// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <engine/framework.h>
#include <lib/logger.h>
#include <lib/thread_pool.h>
#include <precompiled.h>

using namespace ionengine;

LIB_DECLARE_THREAD_POOL()
LIB_INITIALIZE_LOGGER()

Framework::Framework() {
    LIB_INITIALIZE_THREAD_POOL(15)

    _window_loop.emplace();
    _window.emplace("IONENGINE", 800, 600, false);
}

void Framework::run(std::function<void()> const& loop_func) {
    _window_loop->run(*_window, [&](platform::WindowEvent const& event,
                                    platform::WindowEventFlow& flow) {
        flow = platform::WindowEventFlow::Poll;
    });
}
