// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "lib/fmt.h"
#include "lib/math.h"

#include "platform/window.h"

using namespace ionengine;

int32 main(int32, char**) {

    platform::WindowEventLoop window_event_loop;
    window_event_loop.run([&](const platform::WindowEvent& event) -> void { 


    });
}
