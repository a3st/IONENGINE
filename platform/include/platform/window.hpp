// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.h>

namespace ionengine::platform {

///
/// Window class
///
class Window {
 public:
    ///
    /// Get window client width
    /// @return Client width
    ///
    virtual uint32_t width() const = 0;

    ///
    /// Get window client height
    /// @return Client height
    ///
    virtual uint32_t height() const = 0;

    ///
    /// Get window native handle
    /// @return Void pointer
    ///
    virtual void* native_handle() const = 0;

    ///
    /// Set window label
    /// @param label Label string
    ///
    virtual void label(std::string_view const label) = 0;

    ///
    /// Show window cursor
    /// @param show Boolean (true for show, false for hide)
    ///
    virtual void cursor(bool const show) = 0;

    virtual bool has_close() = 0;

    ///
    /// Create window
    /// @param width Width window
    /// @param height Height window
    /// @param label Label window
    /// @return Expected window
    ///
    static core::Expected<std::unique_ptr<Window>, std::string> create(
        uint32_t const width, uint32_t const height,
        std::string_view const label) noexcept;
};

void poll_events(Window& window, std::function<void()> const& loop);

}  // namespace ionengine::platform