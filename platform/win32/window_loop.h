// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "winapi.h"

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

class WindowLoop {

friend class Window;

public:

    WindowLoop();
    WindowLoop(const WindowLoop&) = delete;
    WindowLoop(WindowLoop&&) noexcept = delete;

    WindowLoop& operator=(const WindowLoop&) = delete;
    WindowLoop& operator=(WindowLoop&&) noexcept = delete;

    void Run(const std::function<void(const WindowEvent&)>& run_func);
    
    inline void Quit() { quit_ = true; }

private:

    WindowEvent event_;
    bool quit_;
};

}