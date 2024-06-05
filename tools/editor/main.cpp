// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "view_model.hpp"
#include <core/exception.hpp>

using namespace ionengine;
namespace editor = ionengine::tools::editor;

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        libwebview::App app("ionengine-tools", "IONENGINE Editor", 800, 600, true, true);

        editor::ViewModel view(&app);
        view.run();

        app.setIdle([&]() { view.loop(); });
        app.run("resources/index.html");
        return EXIT_SUCCESS;
    }
    catch (core::Exception e)
    {
        libwebview::showMessageDialog("Crash", e.what(), libwebview::MessageDialogType::Error);
        return EXIT_FAILURE;
    }
}