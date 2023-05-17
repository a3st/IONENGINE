#pragma once

namespace ionengine::platform {

class Window {
public:

    Window();

    Window(Window const&) = delete;

    Window(Window&& other);

    auto operator=(Window const&) -> Window& = delete;

    auto operator=(Window&& other) -> Window&;

    auto clientWidth() const -> uint32_t;

    auto clientHeight() const -> uint32_t;

    auto label(std::string_view const textLabel) -> void;

    auto label() const -> std::string;

    auto cursor(bool show) -> void;

    auto clientSize(uint32_t width, uint32_t height) const -> void;

    auto run(std::function<void()> callback) -> void;

private:

    class Impl* _impl;
};

}