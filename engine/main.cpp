// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <platform/window_loop.h>

#include <renderer/backend.h>
#include <renderer/world_renderer.h>

using namespace ionengine;

int main(int*, char*) {

    platform::WindowLoop loop;
    platform::Window window(u8"IONENGINE", 800, 600, false, &loop);

    renderer::Backend backend(0, &window);
    renderer::WorldRenderer world_renderer(&backend);
    
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