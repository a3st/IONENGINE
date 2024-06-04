// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "compositor.hpp"
#include "engine/engine.hpp"
#include "registry.hpp"
#include <webview.hpp>

namespace ionengine::tools::editor
{
    class ViewModel : public Engine
    {
      public:
        ViewModel(libwebview::App* app);

      protected:
        auto init() -> void override;

        auto update(float const dt) -> void override;

        auto render() -> void override;

      private:
        libwebview::App* app;
        editor::ComponentRegistry componentRegistry;
        editor::DomainRegistry domainRegistry;
    };
} // namespace ionengine::tools::editor