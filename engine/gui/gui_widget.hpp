// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics/camera.hpp"
#include <RmlUi/Core.h>

namespace ionengine
{
    namespace internal
    {
        class RmlRender;
    };

    class GUIWidget : public core::ref_counted_object
    {
      public:
        GUIWidget(internal::RmlRender& rmlRender, Rml::Context* rmlContext, std::filesystem::path const& filePath);

        ~GUIWidget();

        auto getTargetCamera() const -> core::ref_ptr<Camera>;

        auto attachToCamera(core::ref_ptr<Camera> const& targetCamera) -> void;

      private:
        internal::RmlRender* rmlRender;
        Rml::Context* rmlContext;
        Rml::ElementDocument* document;
        core::ref_ptr<Camera> targetCamera;
        std::filesystem::path filePath;
    };
} // namespace ionengine