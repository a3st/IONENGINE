// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

#define NOMINMAX
#define UNICODE
#include "Windows.h"
#undef NOMINMAX

namespace ie {

namespace platform {

class Window : public core::ref_counted_object {
public:

    Window(
        std::string_view const title, 
        uint32_t const width, 
        uint32_t const height
    );

    auto run() -> void;

    auto get_handle() const -> HWND;

private:

    HWND window;

    static auto window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT;
};

}

}