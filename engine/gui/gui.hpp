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

        ~GUI();

        auto onRender() -> void;

        auto onUpdate() -> void;

        auto onResize(uint32_t const width, uint32_t const height) -> void;

      private:
        std::unique_ptr<internal::RmlSystem> rmlSystem;
        std::unique_ptr<internal::RmlRender> rmlRender;

        uint32_t outputWidth;
        uint32_t outputHeight;

        inline static GUI* instance;

      public:
        static auto createWidget(std::filesystem::path const& filePath) -> core::ref_ptr<GUIWidget>;
    };
} // namespace ionengine