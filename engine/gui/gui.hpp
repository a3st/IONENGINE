// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "gui_context.hpp"
#include "rml.hpp"

namespace ionengine
{
    class GUI
    {
      public:
        GUI(core::ref_ptr<rhi::RHI> RHI);

        auto createContext(core::ref_ptr<Camera> targetCamera) -> core::ref_ptr<GUIContext>;

      private:
        std::unique_ptr<internal::RmlRender> rmlRender;

        inline static GUI* instance;

      public:
    };
} // namespace ionengine