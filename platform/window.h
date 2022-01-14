// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/window_loop.h>

namespace ionengine::platform {

class Window {
public:

    Window(std::u8string const& label, uint32_t const width, uint32_t const height, bool const fullscreen, WindowLoop& loop);

    ~Window();

    Window(Window const&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window const&) = delete;

    Window& operator=(Window&&) = delete;

    Size get_size() const;

    void* get_handle() const;

    void set_label(std::u8string const& label);

private:

    struct Impl;
    std::unique_ptr<Impl> _impl;
};

}