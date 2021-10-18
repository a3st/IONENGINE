// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "winapi.h"

namespace ionengine::platform {

class Window {

public:

    Window();
    ~Window();
    Window(const std::string& label, const uint32_t width, const uint32_t height);

    void* GetNativeHandle() const;

    void set_cursor(const bool show) { cursor_ = show; }

private:

    HWND hwnd_;
    
    bool cursor_;

    static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

}