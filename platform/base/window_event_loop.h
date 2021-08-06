// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class WindowEventLoop {
public:

    virtual ~WindowEventLoop() = default;

    virtual void run(const std::function<void(const WindowEventHandler&)>& function_loop) = 0;
    virtual void exit() = 0;
};

}