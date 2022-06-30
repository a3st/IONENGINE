// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <input/input_manager.h>
#include <platform/window.h>
#include <platform/window_loop.h>

namespace ionengine {

class Framework {
 public:
    Framework();

    void run(std::function<void()> const& loop_func);

 private:
    input::InputManager input_manager;

    std::optional<platform::WindowLoop> _window_loop;
    std::optional<platform::Window> _window;
};

}  // namespace ionengine