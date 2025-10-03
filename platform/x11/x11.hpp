// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../platform.hpp"
#include <X11/Xlib.h>

namespace ionengine::platform
{
    class X11App : public App
    {
      public:
        X11App(AppContext& context, std::string_view const title);
        ~X11App();

        auto getWindowHandle() -> void* override;

        auto getInstanceHandle() -> void* override;

        auto run() -> void override;

      private:
        Display* display;
        Window window;
        AppContext* context;

        Atom WM_DELETE_WINDOW;
    };
} // namespace ionengine::platform