// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class WindowEventType {

    Unknown,
    Closed,
    Sized,
    Updated
};

struct WindowEvent {

    WindowEventType type;
};

class IWindowLoop {
public:

    virtual ~IWindowLoop() = default;

    virtual void run(const std::function<void(WindowEvent const&)>& run_func) = 0;

    virtual void quit() = 0;
};

}