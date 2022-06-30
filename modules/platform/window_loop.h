// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class Window;

enum class WindowEventFlow { Unknown, Poll, Exit };

class WindowLoop {
 public:
    WindowLoop() = default;

    WindowLoop(WindowLoop const&) = delete;

    WindowLoop(WindowLoop&&) noexcept {}

    WindowLoop& operator=(WindowLoop const&) = delete;

    WindowLoop& operator=(WindowLoop&&) noexcept {}

    void run(Window& window,
             std::function<void(WindowEvent const&, WindowEventFlow&)> const&
                 run_func);

 private:
    WindowEventFlow _flow{WindowEventFlow::Unknown};
};

}  // namespace ionengine::platform
