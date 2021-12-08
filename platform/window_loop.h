// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/types.h>

namespace ionengine::platform {

enum class WindowEventType {
    Unknown,
    Closed,
    Sized,
    Updated
};

struct WindowEvent {
    WindowEventType type;
    std::variant<Size> data;
};

enum class WindowEventFlow {
    Unknown,
    Poll,
    Exit
};

class WindowLoop {
public:

    WindowLoop() = default;

    WindowLoop(WindowLoop const&) = delete;

    WindowLoop(WindowLoop&&) = delete;

    WindowLoop& operator=(WindowLoop const&) = delete;

    WindowLoop& operator=(WindowLoop&&) = delete;

    void run(std::function<void(WindowEvent const&, WindowEventFlow&)> const& run_func);

    void push_event(WindowEvent const& event);

private:

    WindowEventFlow flow_{WindowEventFlow::Unknown};
    std::queue<WindowEvent> events_;
};

}