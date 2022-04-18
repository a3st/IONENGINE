// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/shader.h>
#include <lib/exception.h>
#include <engine/asset_manager.h>

using namespace ionengine::asset;

Shader::Shader(std::filesystem::path const& file_path, AssetManager& asset_manager) {

    std::string from_path_string = file_path.string();

    JSON_ShaderDefinition document;
    json5::error result = json5::from_file(from_path_string, document);

    if(result != json5::error::none) {
        throw lib::Exception(std::format("File load {}!", from_path_string));
    }

    _name = document.name;

    std::for_each(
        document.parameters.begin(), 
        document.parameters.end(), 
        [&](auto& parameter) { 
            _parameters.emplace_back(parameter.name, static_cast<ShaderParameterType>(parameter.type), parameter.from); 
        }
    );
    
    std::for_each(
        document.passes.begin(), 
        document.passes.end(), 
        [&](auto& pass) { 

            auto draw_parameters = DrawParameters {
                .fill_mode = static_cast<DrawParameterFillMode>(pass.draw_parameters.fill_mode),
                .cull_mode = static_cast<DrawParameterCullMode>(pass.draw_parameters.cull_mode),
                .depth_stencil = pass.draw_parameters.depth_stencil
            };
            
            std::shared_ptr<Technique> technique = asset_manager.load<Technique>(pass.technique.from);
            _shader_passes.emplace_back(pass.name, draw_parameters, technique);
        }
    );
}

std::string_view Shader::name() const {

    return _name;
}

std::span<ShaderParameter const> Shader::parameters() const {
    
    return _parameters;
}

std::span<ShaderPass const> Shader::shader_passes() const {
    
    return _shader_passes;
}
