// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"

#include "../platform/platform.h"
#include "../rendersystem/rendersystem.h"

using namespace ionengine;

size_t allocate = 0;
size_t deallocate = 0;

std::wstring label;

void* operator new(size_t size) {
    ++allocate;
    void* p = malloc(size);
    return p;
}
 
void operator delete(void * p) {
    ++deallocate;
    free(p);
}

int main(int*, char*) {

    label.reserve(50);
    uint64_t value = 0;

    platform::WindowLoop loop;
    platform::Window window("IONENGINE", 800, 600, &loop);
    rendersystem::Renderer renderer(&window);
    
    loop.Run([&](const platform::WindowEvent& event) {
            switch(event.type) {
                case platform::WindowEventType::Closed: loop.Quit(); break;
                case platform::WindowEventType::Sized: {
                    
                    break;
                }
                case platform::WindowEventType::Updated: {
                    renderer.Frame();
                    ++value;

                    if(value >= 10000) {
                        swprintf(label.data(), L"IONENGINE [Alloc: %zi/%zi]", allocate, deallocate);
                        SetWindowText(reinterpret_cast<HWND>(window.GetNativeHandle()), label.c_str());
                        value = 0;
                    }
                    break;
                }
            }
        });
    return 0;
}