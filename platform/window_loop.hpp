// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "platform/window.hpp"

namespace ionengine {

namespace platform {

enum class WindowEventFlow {
    Poll,
    Exit
};

using window_callback_func_t = std::function<void(WindowEvent const&, WindowEventFlow&)>;

class WindowLoop : public core::ref_counted_object {
public:

    WindowLoop() = default;

    void run(Window& window, window_callback_func_t const& callback);

private:

    WindowEventFlow flow{WindowEventFlow::Poll};
};

}

}