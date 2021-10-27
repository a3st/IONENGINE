// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "d3d12.h"

namespace lgfx {

struct HANDLE_deleter {
    
    void operator()(HANDLE handle) {

        CloseHandle(handle);
    }
};

typedef std::unique_ptr<std::remove_pointer<HANDLE>::type, HANDLE_deleter> UniqueHANDLE;

}