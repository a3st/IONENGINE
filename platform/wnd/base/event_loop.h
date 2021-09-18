// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform::wnd {

/**
    @brief Window event loop class

    It's need for creation application event loop 
*/
class WindowEventLoop {
public:

    virtual ~WindowEventLoop() = default;

    /**
        @brief Run event loop
        @param loop reference to lambda function
    */
    virtual void run(const std::function<void(const WindowEventHandler&)>& loop) = 0;

    /**
        @brief Break event loop
    */
    virtual void exit() = 0;
};

}