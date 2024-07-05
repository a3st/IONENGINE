// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "conv/texture.hpp"
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
        libraryDirPath = "E:/GitHub/IONENGINE/assets";
    }

    if (commandLine[{"-c", "--convert"}])
    {
        std::cout << "Tools for IONENGINE > Resource Compiler\n";
        std::cout << "Copyright (R) Dmitriy Lukovenko. All rights reserved.\n" << std::endl;

        std::string input;
        if (!(commandLine(1) >> input))
        {
            std::cerr << "ERROR: Missing input file" << std::endl;
            return EXIT_SUCCESS;
        }

        std::string output;
        if (!(commandLine({"-output", "--output"}) >> output))
        {
            output =
                (std::filesystem::path(input).parent_path() / std::filesystem::path(input).stem()).string() + ".asset";
        }

        editor::TextureImport textureImport;
        auto result = textureImport.loadFromFile(std::filesystem::path(input).make_preferred());
        if (result.has_value())
        {
            if (core::saveToFile<TextureFile, core::serialize::OutputArchive>(
                    result.value(), std::filesystem::path(output).make_preferred()))
            {
                std::cout << "Out: " << std::filesystem::absolute(output).generic_string() << std::endl;
            }
            else
            {
                std::cerr << "Compilation error: Resource is corrupted" << std::endl;
            }
        }
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