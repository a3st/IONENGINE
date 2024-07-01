// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "view_model.hpp"
#include <argh.h>
#include <core/exception.hpp>

using namespace ionengine;
namespace editor = ionengine::tools::editor;

auto main(int32_t argc, char** argv) -> int32_t
{
    argh::parser commandLine(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

    std::string libraryDirPath;
    if (!(commandLine({"-lp", "--libpath"}) >> libraryDirPath))
    {
        libraryDirPath = "E:/GitHub/IONENGINE/build/assets";
    }

    std::string consoleMode;
    if (commandLine({"-con", "--console"}) >> consoleMode)
    {
        
    }
    else
    {
        try
        {
            libwebview::App app("ionengine-tools", "IONENGINE Editor", 800, 600, true, true);

            editor::ViewModel view(&app, std::filesystem::path(libraryDirPath).make_preferred());
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
}