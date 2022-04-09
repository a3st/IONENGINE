// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class WindowEventType {
    Unknown,
    Closed,
    Sized,
    Updated
};

struct Size {
    uint32_t width;
    uint32_t height;
};

struct WindowEvent {
    WindowEventType type;
    std::variant<Size> data;
};

class Window {
public:

    Window(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen);

    ~Window();

    Window(Window const&) = delete;

    Window(Window&&) { }

    Window& operator=(Window const&) = delete;

    Window& operator=(Window&&) { }

    Size client_size() const;

    void* native_handle() const;

    void label(std::string_view const label);

    std::queue<WindowEvent>& messages();

private:

    struct Impl;
    struct impl_deleter { void operator()(Impl* ptr) const; };
    std::unique_ptr<Impl, impl_deleter> _impl;
};

}
