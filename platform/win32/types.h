// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class WindowsWindowEventLoop;
class WindowsWindow;

enum class WindowStyle : uint32 {
    Normal = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
    Minimize = WS_MINIMIZEBOX,
    Maximaze = WS_MAXIMIZEBOX
};


enum class MessageBoxStyle : uint32 {
    ButtonOK = MB_OK,
    IconError = MB_ICONERROR,
    IconWarning = MB_ICONINFORMATION
};

}