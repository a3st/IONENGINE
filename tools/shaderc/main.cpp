// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "core/exception.hpp"
#include "fx.hpp"
#include "precompiled.h"
#include <argh.h>

namespace shaderc = ionengine::tools::shaderc;

auto main(int32_t argc, char** argv) -> int32_t
{
    argh::parser commandLine(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

    std::cout << "Tools for IONENGINE > Shader Compiler" << std::endl;
    std::cout << "Copyright (R) Dmitriy Lukovenko. All rights reserved." << std::endl;
    std::cout << std::endl;

    if (commandLine[{"-help", "--help"}])
    {
        std::cout << "usage: shaderc <command> [arguments] input_file" << std::endl;
        std::cout << std::endl;
        std::cout << "-target (--target)" << "\t\t\t" << "Compilation target" << std::endl;
        std::cout << "\t\t\t\t\t" << "Available parameters: SPIRV, DXIL" << std::endl << std::endl;
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
        shaderc::FXCompiler fxCompiler;
        fxCompiler.addIncludePath(std::filesystem::path(input).parent_path());

        if (target.compare("DXIL") == 0)
        {
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
        if (fxCompiler.compile(input, output, errors))
        {
            std::cout << "Out: " << std::filesystem::path(output).generic_string() << std::endl;
        }
        else
        {
            std::cerr << "Compilation error: " << errors << std::endl;
        }
        return EXIT_SUCCESS;
    }
    catch (ionengine::core::Exception e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}