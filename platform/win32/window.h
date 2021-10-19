// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "winapi.h"

namespace ionengine::platform {

enum class WindowEventType {
    Closed,
    Sized
};

class Window {

public:

    Window();
    ~Window();
    Window(const std::string& label, const uint32_t width, const uint32_t height);

    inline void* GetNativeHandle() const { return reinterpret_cast<HWND>(hwnd_); }

    inline void SetCursor(const bool show) { cursor_ = show; }
    inline bool GetCursor() { return cursor_; }

    inline uint32_t GetWidth() { return width_; }
    inline uint32_t GetHeight() { return height_; }

private:

    HWND hwnd_;
    
    bool cursor_;

    uint32_t width_;
    uint32_t height_;

    static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

}