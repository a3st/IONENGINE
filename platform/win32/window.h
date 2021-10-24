// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "winapi.h"
#include "hwnd_ptr.h"

namespace ionengine::platform {

class WindowLoop;

class Window {

public:

    Window();
    Window(const std::string& label, const uint32_t width, const uint32_t height, WindowLoop* loop);
    Window(const Window&) = delete;
    Window(Window&& rhs) noexcept;

    Window& operator=(const Window&) = delete;
    Window& operator=(Window&& rhs) noexcept;

    inline void* GetNativeHandle() const { return reinterpret_cast<void*>(hwnd_.get()); }

    void SetLabel(const std::string& label);

    inline void SetCursor(const bool show) { cursor_ = show; }
    inline bool GetCursor() { return cursor_; }

    inline uint32_t GetWidth() { return width_; }
    inline uint32_t GetHeight() { return height_; }

private:

    UniqueHWND hwnd_;
    
    bool cursor_;

    uint32_t width_;
    uint32_t height_;

    WindowLoop* loop_;

    static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

}