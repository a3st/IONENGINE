// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <hlsl_spirv_package/hlsl_spirv.h>
#include <tclap/CmdLine.h>
#include <lib/algorithm.h>

using namespace hlsl_spirv_package;

const uint32_t MAJOR_VERSION = 1;
const uint32_t MINOR_VERSION = 0;

int main(int argc, char** argv) {

    std::cout << std::format("Welcome to HLSL SPIRV Package Tool [Version {}.{}]", MAJOR_VERSION, MINOR_VERSION) << std::endl;
    std::cout << "(c) 2022 Dmitriy Lukovenko. All rights reserved" << std::endl;

    std::filesystem::path output_path = "output_package.hlsv";
    std::string flags = "all";
    std::vector<std::filesystem::path> shader_files;

    try {

        TCLAP::CmdLine cmd_line("", ' ', std::format("{}.{}", MAJOR_VERSION, MINOR_VERSION));

        TCLAP::ValueArg<std::filesystem::path> _output_path("o", "output-path", "Output file path", false, "output_package.hlsv", "path");
        cmd_line.add(_output_path);

        TCLAP::ValueArg<std::string> _flags("f", "flags", "Flags for package (hlsl-only, spirv-only or all)", false, "all", "string");
        cmd_line.add(_flags);

        TCLAP::UnlabeledMultiArg<std::filesystem::path> _shader_files("file-path", "Shader files", true, "", "path");
        cmd_line.add(_shader_files);

        cmd_line.parse(argc, argv);

        output_path = _output_path.getValue();
        flags = _flags.getValue();
        shader_files = _shader_files.getValue();

    } catch(TCLAP::ArgException& exception) {
        std::cerr << std::format("[Error] {} for argument {}", exception.error(), exception.argId()) << std::endl;
        return EXIT_FAILURE;
    }

    HLSL_SPIRV_Flags _flags{};

    if(flags.contains("all")) {
        HLSL_SPIRV_Flags _flags = HLSL_SPIRV_Flags::HLSL | HLSL_SPIRV_Flags::SPIRV;
    } else if(flags.contains("hlsl-only")) {
        HLSL_SPIRV_Flags _flags = HLSL_SPIRV_Flags::HLSL;
    } else if(flags.contains("spirv-only")) {
        HLSL_SPIRV_Flags _flags = HLSL_SPIRV_Flags::SPIRV;
    } else {
        std::cerr << std::format("[Error] {} for argument {}", "Invalid value", "-f, --flags") << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::format("[Info] Packaging {} shader files (FLAGS={})", shader_files.size(), flags) << std::endl;

    auto header = HLSL_SPIRV_Header {};
    uint32_t magic = HLSL_SPIRV_Magic;
    header.flags = std::to_underlying(_flags);
    header.count = static_cast<uint32_t>(shader_files.size());

    std::unique_ptr<HLSL_SPIRV_Shader[]> shaders = std::make_unique<HLSL_SPIRV_Shader[]>(shader_files.size());
    std::unique_ptr<char8_t[]> data = std::make_unique<char8_t[]>(2 * 1024 * 1024 * shader_files.size());

    uint64_t offset = 0;

    std::vector<char8_t> shader_data;

    for(size_t i = 0; i < shader_files.size(); ++i) {

        std::u8string file_path_u8string = shader_files[i].filename().u8string();
        std::memcpy(shaders[i].name, file_path_u8string.data(), file_path_u8string.size() * sizeof(char8_t));

        if(shader_files[i].extension() == ".hlsl_bin") {
            shaders[i].flags = std::to_underlying(HLSL_SPIRV_Flags::HLSL);
        } else if(shader_files[i].extension() == ".spirv_bin") {
            shaders[i].flags = std::to_underlying(HLSL_SPIRV_Flags::SPIRV);
        } else {
            std::cerr << std::format("[Error] {}", "Failed to recognize shader format (.hlsl_bin or .spirv_bin only)") << std::endl;
            return EXIT_FAILURE;
        }

        size_t file_size = ionengine::get_file_size(shader_files[i], std::ios::binary);
        shader_data.resize(file_size);

        shaders[i].size = file_size;

        if(!ionengine::load_bytes_from_file(shader_files[i], shader_data, std::ios::binary)) {
            std::cerr << std::format("[Error] {} ({})", "Failed to read shader data", shader_files[i].filename().string()) << std::endl;
            return EXIT_FAILURE;
        }

        std::memcpy(data.get() + offset, shader_data.data(), file_size);
        offset += file_size;
    }

    std::vector<char8_t> final_data;

    size_t const header_size = sizeof(HLSL_SPIRV_Header);
    size_t const shader_size = sizeof(HLSL_SPIRV_Shader) * shader_files.size();
    size_t const data_size = offset;
    size_t const total_bytes = 
        sizeof(uint32_t) + // Magic size
        header_size + // Header size
        shader_size + // Shader size
        data_size // Data size
    ;
    final_data.resize(total_bytes);

    offset = 0;

    std::memcpy(final_data.data(), &magic, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    std::memcpy(final_data.data() + offset, &header, header_size);
    offset += header_size;

    std::memcpy(final_data.data() + offset, shaders.get(), shader_size);
    offset += shader_size;

    std::memcpy(final_data.data() + offset, data.get(), data_size);
    offset += data_size;
    
    if(!ionengine::save_bytes_to_file(output_path, final_data)) {
        std::cerr << std::format("[Error] {}", "Failed to save shader package") << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[Info] Shader packaging completed successfully" << std::endl;
    return EXIT_SUCCESS;
}
