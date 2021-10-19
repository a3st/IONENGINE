// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"

#include "platform/platform.h"

using namespace ionengine;

int main(int*, char*) {

    platform::WindowLoop loop;
    platform::Window window("IONENGINE", 800, 600, &loop);
    
    loop.Run([&](const platform::WindowEvent& event) {
            switch(event.type) {
                case platform::WindowEventType::Closed: loop.Quit(); break;
                case platform::WindowEventType::Sized: {
                    
                    break;
                }
                case platform::WindowEventType::Updated: {
                    
                    break;
                }
            }
        });
    return 0;
}