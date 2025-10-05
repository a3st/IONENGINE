// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "shadersys/compiler.hpp"
#include <argh.h>

using namespace ionengine;

auto main(int32_t argc, char** argv) -> int32_t
{
    argh::parser commandLine(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

    std::cout << "Tools for IONENGINE > Shader Compiler\n";
    std::cout << "Copyright (R) Dmitriy Lukovenko. All rights reserved.\n" << std::endl;

    if (commandLine[{"-help", "--help"}])
    {
        std::cout << "usage: shaderc <inputFile> [arguments]\n\n";
        std::cout << "-target (--target)" << "\t\t\t" << "Compilation target\n";
        std::cout << "\t\t\t\t\t" << "Available parameters: SPIRV, DXIL\n\n";
        std::cout << "-output (--output)" << "\t\t\t" << "Compilation output path (Optional)" << std::endl;
        return EXIT_SUCCESS;
    }

    std::string target;
    if (!(commandLine({"-target", "--target"}) >> target))
    {
        std::cerr << "ERROR: Missing parameters (-target, --target)" << std::endl;
        return EXIT_SUCCESS;
    }

    std::filesystem::path inputPath;
    {
        std::string input;
        if (!(commandLine(1) >> input))
        {
            std::cerr << "ERROR: Missing input file" << std::endl;
            return EXIT_SUCCESS;
        }

        inputPath = std::filesystem::path(input);
    }

    std::filesystem::path outputPath;
    {
        std::string output;
        if (!(commandLine({"-output", "--output"}) >> output))
        {
            output = (inputPath.parent_path() / inputPath.stem()).string() + ".bin";
        }

        outputPath = std::filesystem::path(output);
    }

    try
    {
        core::ref_ptr<shadersys::ShaderCompiler> shaderCompiler;
        std::filesystem::path const includeBasePath = inputPath.parent_path() / "include";
        if (target.compare("DXIL") == 0)
        {
            shaderCompiler = shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::DXIL, includeBasePath);
        }
        else if (target.compare("SPIRV") == 0)
        {
            shaderCompiler = shadersys::ShaderCompiler::create(asset::fx::ShaderFormat::SPIRV, includeBasePath);
        }
        else
        {
            std::cerr << "ERROR: Invalid target parameter (see -help, --help)" << std::endl;
            return EXIT_SUCCESS;
        }

        auto compileResult = shaderCompiler->compile(inputPath);
        if (compileResult.has_value())
        {
            std::basic_ofstream<uint8_t> ofs(outputPath, std::ios::binary);
            auto serializeResult = core::serialize<core::serialize_oarchive>(ofs, compileResult.value());
            if (serializeResult.has_value())
            {
                std::cout << "Out: " << std::filesystem::absolute(outputPath).generic_string() << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: " << compileResult.error().what() << std::endl;
        }
        return EXIT_SUCCESS;
    }
    catch (std::runtime_error e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}