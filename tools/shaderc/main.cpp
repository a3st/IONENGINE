// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

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
        std::cout << "usage: shaderc <command> [arguments] input_file\n\n";
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

    std::string input;
    if (!(commandLine(1) >> input))
    {
        std::cerr << "ERROR: Missing input file" << std::endl;
        return EXIT_SUCCESS;
    }

    std::string output;
    if (!(commandLine({"-output", "--output"}) >> output))
    {
        output = (std::filesystem::path(input).parent_path() / std::filesystem::path(input).stem()).string() + ".bin";
    }

    try
    {
        core::ref_ptr<shadersys::ShaderCompiler> shaderCompiler;

        if (target.compare("DXIL") == 0)
        {
            shaderCompiler = shadersys::ShaderCompiler::create(shadersys::fx::ShaderAPIType::DXIL);
        }
        else if (target.compare("SPIRV") == 0)
        {
            std::cout << "WARNING: This target is not currently supported by the compiler" << std::endl;
            return EXIT_SUCCESS;
        }
        else
        {
            std::cerr << "ERROR: Invalid target parameter (see -help, --help)" << std::endl;
            return EXIT_SUCCESS;
        }

        std::string errors;
        auto compileResult = shaderCompiler->compileFromFile(std::filesystem::path(input).make_preferred(), errors);
        if (compileResult.has_value())
        {
            core::to_file<shadersys::ShaderEffectFile, core::serialize_oarchive>(
                compileResult.value(), std::filesystem::path(output).make_preferred());

            std::cout << "Out: " << std::filesystem::absolute(output).generic_string() << std::endl;
        }
        else
        {
            std::cerr << "Compilation error: " << errors << std::endl;
        }
        return EXIT_SUCCESS;
    }
    catch (core::runtime_error e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}