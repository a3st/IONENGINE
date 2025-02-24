// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/platform.hpp"

namespace ionengine
{
    class Window : public core::ref_counted_object
    {
      public:
        Window(core::ref_ptr<platform::App> application);

        auto setEnableMouse(bool const isEnable) -> void;

      private:
        core::ref_ptr<platform::App> application;
    };
} // namespace ionengine