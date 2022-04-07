// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/window_loop.h>

namespace ionengine::platform {

class Window {
public:

    Window(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen, WindowLoop& loop);

    ~Window();

    Window(Window const&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window const&) = delete;

    Window& operator=(Window&&) = delete;

    Size client_size() const;

    void* native_handle() const;

    void label(std::string_view const label);

private:

    struct Impl;
    struct impl_deleter { void operator()(Impl* ptr) const; };
    std::unique_ptr<Impl, impl_deleter> _impl;
};

}
