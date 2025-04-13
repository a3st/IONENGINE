// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "gui_widget.hpp"
#include "rml.hpp"

namespace ionengine
{
    class GUI
    {
      public:
        GUI(core::ref_ptr<rhi::RHI> RHI);

      private:
        std::unique_ptr<internal::RmlRender> rmlRender;
        std::unique_ptr<internal::RmlSystem> rmlSystem;

        inline static GUI* instance;

      public:
        static auto createWidgetFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<GUIWidget>;
    };
} // namespace ionengine