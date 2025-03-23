// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/platform.hpp"

namespace ionengine
{
    class Window
    {
      public:
        Window(core::ref_ptr<platform::App> application);

      private:
        core::ref_ptr<platform::App> application;

        inline static Window* instance;

      public:
        static auto setEnableMouse(bool const isEnable) -> void;
    };
} // namespace ionengine