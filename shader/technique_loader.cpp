

#include <precompiled.h>
#include <shader/technique_loader.h>

#include <lib/algorithm.h>

using namespace ionengine::shader;

bool TechniqueLoader::parse(std::span<uint8_t const> const data) {

    std::string_view string_view(reinterpret_cast<char const*>(data.data()), reinterpret_cast<char const*>(data.data()) + data.size());

    TechniqueData technique_data;
    json5::error result = json5::from_string(string_view, technique_data);

    if(result != json5::error::none) {
        return false;
    }

    std::unordered_map<ShaderType, std::vector<std::string>> uniforms_gen;

    for(auto& uniform : technique_data.uniforms) {

        std::string uniform_gen;

        switch(uniform.type) {

            case UniformType::cbuffer: {
                uniform_gen += "cbuffer";
                uniform_gen += " ";
                uniform_gen += uniform.name;
                uniform_gen += " : ";
                uniform_gen += "register";
                uniform_gen += std::format("(b{})", uniform.location);
                uniform_gen += " {";
                uniform_gen += "\n";

                for(auto& declaration : uniform.declaration.value()) {

                    uniform_gen += "\t";
                    switch(declaration.type) {
                        case DataType::f32x4x4: { uniform_gen += "float4x4"; } break;
                        case DataType::f32x3: { uniform_gen += "float3"; } break;
                        case DataType::f32x2: { uniform_gen += "float2"; } break;
                        case DataType::f32: { uniform_gen += "float"; } break;
                    }
                    uniform_gen += " ";
                    uniform_gen += declaration.name;
                    uniform_gen += ";\n";
                }

                uniform_gen += "\n};\n";
            } break;

            case UniformType::sampler2D: {
                uniform_gen += "SamplerState";
                uniform_gen += " ";
                uniform_gen += uniform.name;
                uniform_gen += " : ";
                uniform_gen += "register";
                uniform_gen += std::format("(s{});", uniform.location);

                uniform_gen += "Texture2D";
                uniform_gen += " ";
                uniform_gen += uniform.name;
                uniform_gen += " : ";
                uniform_gen += "register";
                uniform_gen += std::format("(t{});", uniform.location);
            } break;
        }

        uniforms_gen[uniform.visibility.value()].emplace_back(uniform_gen);
    }

    std::vector<std::string> shader_codes;

    for(auto& shader : technique_data.shaders) {

        std::stringstream shader_gen;

        shader_gen << "struct vs_input {\n";
        for(auto& input : shader.inputs) {

            shader_gen << "\t";

            switch(input.type) {
                case DataType::f32x4x4: { shader_gen << "float4x4"; } break;
                case DataType::f32x4: { shader_gen << "float4"; } break;
                case DataType::f32x3: { shader_gen << "float3"; } break;
                case DataType::f32x2: { shader_gen << "float2"; } break;
                case DataType::f32: { shader_gen << "float"; } break;
            }

            shader_gen << " : ";
            shader_gen << input.semantic;
            shader_gen << ";\n";
        }
        shader_gen << "\n};\n";

        shader_gen << "\n";

        shader_gen << "struct vs_output {\n";
        for(auto& output : shader.outputs) {

            shader_gen << "\t";

            switch(output.type) {
                case DataType::f32x4x4: { shader_gen << "float4x4"; } break;
                case DataType::f32x4: { shader_gen << "float4"; } break;
                case DataType::f32x3: { shader_gen << "float3"; } break;
                case DataType::f32x2: { shader_gen << "float2"; } break;
                case DataType::f32: { shader_gen << "float"; } break;
            }
            shader_gen << " ";
            shader_gen << output.name;

            shader_gen << " : ";
            shader_gen << output.semantic;
            shader_gen << ";\n";
        }
        shader_gen << "\n};\n";

        shader_gen << "\n";

        for(auto& uniform : uniforms_gen[shader.type]) {
            
            shader_gen << uniform;
            shader_gen << "\n";
        }

        shader_gen << shader.source;

        shader_codes.emplace_back(shader_gen.str());
    }

    std::cout << std::format("{}", shader_codes[0]) << std::endl;

    lib::save_bytes_to_file("geometry_vs.hlsl", std::span<uint8_t const>((uint8_t const*)shader_codes[0].data(), shader_codes[0].size()));

    return true;
}