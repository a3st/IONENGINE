// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "registry.hpp"
#include "merger.hpp"
#include <webview.hpp>

namespace ionengine::tools::editor
{
    class App
    {
      public:
        App(int32_t argc, char** argv);

        auto run() -> int32_t;

      private:
        libwebview::App app;
        editor::ComponentRegistry registry;
        editor::Merger merger;

        auto registerComponents() -> void;
    };
} // namespace ionengine::tools::editor