// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class WindowEventType {
    Unknown,
    Closed,
    Sized,
    Updated
};

enum class WindowEventFlow {
    Unknown,
    Poll,
    Exit
};

struct Size {
    uint32_t width;
    uint32_t height;
};

struct WindowEvent {
    WindowEventType type;
    std::variant<Size> data;
};

class WindowLoop {
public:

    WindowLoop() = default;

    WindowLoop(WindowLoop const&) = delete;

    WindowLoop(WindowLoop&&) noexcept = delete;

    WindowLoop& operator=(WindowLoop const&) = delete;

    WindowLoop& operator=(WindowLoop&&) noexcept = delete;

    void run(std::function<void(WindowEvent const&, WindowEventFlow&)> const& run_func);

private:

    std::queue<WindowEvent> _events;
    WindowEventFlow _flow{WindowEventFlow::Unknown};

    friend class Window;
};

}
