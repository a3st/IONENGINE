
#pragma once

#include "platform/window.h"

#define NOMINMAX
#define UNICODE
#include <windows.h>

namespace ionengine::platform {

auto getWindowHandle(Window const& window) -> HWND*;

namespace internal {

class WindowImplWin32 {
public:

    WindowImplWin32();

    auto clientSize(uint32_t width, uint32_t height) -> void;
    
    auto label(std::string const textLabel) -> void;

    static LRESULT windowCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    

private:

    HWND* _window;
    HINSTANCE* _instance;
};

}

}