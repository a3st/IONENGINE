// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <tclap/CmdLine.h>
#include <lib/algorithm.h>
#include <asset_compiler/asset_compiler.h>

uint32_t const MAJOR_VERSION = 1;
uint32_t const MINOR_VERSION = 0;

int main(int argc, char** argv) {

    std::cout << std::format("Welcome to Asset Compiler Tool [Version {}.{}]", MAJOR_VERSION, MINOR_VERSION) << std::endl;
    std::cout << "(c) 2022 Dmitriy Lukovenko. All rights reserved" << std::endl;

    std::filesystem::path output_path = "out";
    std::vector<std::filesystem::path> compile_files;

    try {

        TCLAP::CmdLine cmd_line("", ' ', std::format("{}.{}", MAJOR_VERSION, MINOR_VERSION));

        TCLAP::ValueArg<std::filesystem::path> _output_path("o", "output-path", "Output folder path", false, "out", "path");
        cmd_line.add(_output_path);

        TCLAP::UnlabeledMultiArg<std::filesystem::path> _compile_files("file-path", "Compile files", true, "", "path");
        cmd_line.add(_compile_files);

        cmd_line.parse(argc, argv);

        output_path = _output_path.getValue();
        compile_files = _compile_files.getValue();

    } catch(TCLAP::ArgException& exception) {
        std::cerr << std::format("[Error] {} for argument {}", exception.error(), exception.argId()) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::format("[Info] Compiling {} files", compile_files.size()) << std::endl;

    ionengine::tools::AssetCompiler asset_compiler;

    for(auto& compile_file : compile_files) {

        ionengine::AssetFile asset_file;
        if(!asset_compiler.compile(compile_file, asset_file)) {
            std::cerr << std::format("[Error] {} '{}'", "An error occurred while compiling the file", compile_file.string()) << std::endl;
            continue;
        }

        size_t const file_size = asset_compiler.serialize(nullptr, asset_file);

        std::vector<char8_t> serialized_data;
        serialized_data.resize(file_size);

        std::span<char8_t> span_data = serialized_data;
        if(!(asset_compiler.serialize(&span_data, asset_file) > 0)) {
            std::cerr << std::format("[Error] {} '{}'", "An error occurred while serialize the file", compile_file.string()) << std::endl;
            continue;
        }

        if(!ionengine::save_bytes_to_file(output_path / compile_file.stem().concat(".asset"), span_data, std::ios::binary)) {
            std::cerr << std::format("[Error] {} '{}'", "An error occurred while save the file", compile_file.string()) << std::endl;
            continue;
        }
    }

    std::cout << "[Info] Compilation of assets completed successfully" << std::endl;
    return EXIT_SUCCESS;
}
