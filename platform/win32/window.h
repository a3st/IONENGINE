// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#define NOMINMAX
#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#include <common/platform/window.h>
#include <common/platform/window_loop.h>

namespace ionengine::platform {

class Window : public IWindow {
public:

    Window(std::string const& label, uint32_t const width, uint32_t const height, WindowLoop* const loop);

    void* get_handle() const override { return reinterpret_cast<void*>(hwnd_.get()); }

    void set_label(std::string const& label) override;

    void set_cursor(bool const show) override { cursor_ = show; }
    
    bool get_cursor() const override { return cursor_; }

    Size get_size() const override { return size_; }

private:

    HWND hwnd_;
    bool cursor_;
    Size size_;
    WindowLoop* loop_;

    static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

}