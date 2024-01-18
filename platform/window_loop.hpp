// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "platform/window.hpp"

namespace ionengine::platform {

enum class WindowEventFlow {
    Unknown,
    Poll,
    Exit
};

class WindowLoop : public core::ref_counted_object {
public:

    WindowLoop() = default;

    WindowLoop(WindowLoop const&) = delete;

    WindowLoop(WindowLoop&&) noexcept { }

    WindowLoop& operator=(WindowLoop const&) = delete;

    WindowLoop& operator=(WindowLoop&&) noexcept { }

    void run(Window& window, std::function<void(WindowEvent const&, WindowEventFlow&)> const& run_func);

private:

    WindowEventFlow flow{WindowEventFlow::Unknown};
};

}
