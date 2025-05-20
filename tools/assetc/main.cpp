// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "mdl/obj/obj.hpp"
#include "precompiled.h"
#include "txe/cmp/cmp.hpp"
#include <argh.h>

using namespace ionengine;

auto main(int32_t argc, char** argv) -> int32_t
{
    argh::parser commandLine(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

    std::cout << "Tools for IONENGINE > Asset Compiler\n";
    std::cout << "Copyright (R) Dmitriy Lukovenko. All rights reserved.\n" << std::endl;

    if (commandLine[{"-help", "--help"}])
    {
        std::cout << "usage: assetc <inputFile> [arguments]\n\n";
        std::cout << "-output (--output)" << "\t\t\t" << "Compilation output path (Optional)" << std::endl;
        return EXIT_SUCCESS;
    }

    std::filesystem::path inputPath;
    {
        std::string input;
        if (!(commandLine(1) >> input))
        {
            std::cerr << "ERROR: Missing the input file" << std::endl;
            return EXIT_SUCCESS;
        }

        inputPath = std::filesystem::path(input).make_preferred();
    }

    std::filesystem::path outputPath;
    {
        std::string output;
        if (!(commandLine({"-output", "--output"}) >> output))
        {
            output = (inputPath.parent_path() / inputPath.stem()).string();
        }

        outputPath = std::filesystem::path(output).make_preferred();
    }

    try
    {
        if (inputPath.extension().compare(".obj") == 0)
        {
            auto objImporter = core::make_ref<asset::OBJImporter>();
            auto loadResult = objImporter->loadFromFile(inputPath);
            if (loadResult.has_value())
            {
                std::basic_ofstream<uint8_t> ofs(outputPath.string() + ".mdl", std::ios::binary);
                auto serializeResult = core::serialize<core::serialize_oarchive>(ofs, loadResult.value());
                if (serializeResult.has_value())
                {
                    std::cout << "Out: " << std::filesystem::absolute(outputPath).generic_string() + ".mdl" << std::endl;
                }
            }
            else
            {
                std::cerr << "Compilation error: " << loadResult.error().what() << std::endl;
            }
        }
        else if (inputPath.extension().compare(".png") == 0)
        {
            auto cmpImporter = core::make_ref<asset::CMPImporter>(true);
            auto loadResult = cmpImporter->loadFromFile(inputPath);
            if (loadResult.has_value())
            {
                std::basic_ofstream<uint8_t> ofs(outputPath.string() + ".txe", std::ios::binary);
                auto serializeResult = core::serialize<core::serialize_oarchive>(ofs, loadResult.value());
                if (serializeResult.has_value())
                {
                    std::cout << "Out: " << std::filesystem::absolute(outputPath).generic_string() + ".txe" << std::endl;
                }
            }
            else
            {
                std::cerr << "Compilation error: " << loadResult.error().what() << std::endl;
            }
        }
        return EXIT_SUCCESS;
    }
    catch (std::runtime_error e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}