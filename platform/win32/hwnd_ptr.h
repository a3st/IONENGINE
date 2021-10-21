// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "winapi.h"

namespace ionengine::platform {

struct HWND_deleter {
    void operator()(HWND hwnd) {

        DestroyWindow(hwnd);
    }
};

typedef std::unique_ptr<std::remove_pointer<HWND>::type, HWND_deleter> UniqueHWND;

}
