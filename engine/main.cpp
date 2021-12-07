// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>

using namespace ionengine;

int main(int*, char*) {

    platform::WindowLoop loop;
    platform::Window window(u8"IONENGINE", 800, 600, false, &loop);
    
    loop.run(
        [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {

            flow = platform::WindowEventFlow::Poll;

            switch(event.type) {
                case platform::WindowEventType::Closed: { flow = platform::WindowEventFlow::Exit; break; }
            }
        }
    );
    return 0;
}