// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#define NOMINMAX
#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#include "precompiled.h"

#include "runtime.h"
#include "framework/app_framework.h"
#include "renderer/renderer.h"

using namespace ionengine;

std::unique_ptr<EngineEnv> ionengine::g_env;

LRESULT wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    
    switch(msg) {

		case WM_CLOSE: {

            
            break;
        }
        case WM_SIZE: {
			
            g_env->app_framework->on_window_resize(LOWORD(lParam), HIWORD(lParam));

			break;
		}
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int32 main(int32 argc, char** argv) {
		
	WNDCLASS wnd_class{};
	wnd_class.lpszClassName = TEXT("IONENGINE");
	wnd_class.hInstance = GetModuleHandle(nullptr);
	wnd_class.lpfnWndProc = wnd_proc;

	if (!RegisterClass(&wnd_class)) {
		throw std::runtime_error("An error occurred while registering the window");
	}

	HWND hwnd = CreateWindow(
		wnd_class.lpszClassName,
		L"IONENGINE",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		0, 100,
		800, 600,
		nullptr, nullptr,
		wnd_class.hInstance,
		nullptr
	);

	if (!hwnd) {
		throw std::runtime_error("An error occurred while creating the window");
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);

    g_env = std::make_unique<EngineEnv>();
    
	std::unique_ptr<framework::AppFramework> app_framework = std::make_unique<framework::AppFramework>(reinterpret_cast<void*>(hwnd), 800, 600);
    std::unique_ptr<renderer::Renderer> renderer = std::make_unique<renderer::Renderer>(app_framework.get());

    g_env->app_framework = app_framework.get();
    g_env->renderer = renderer.get();

	MSG msg{};
	while (true) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
