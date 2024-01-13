// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/material.h>
#include <asset/shader.h>
#include <asset/texture.h>
#include <lib/exception.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::asset;

lib::Expected<Material, lib::Result<MaterialError>> Material::load_from_file(std::filesystem::path const& file_path, AssetManager& asset_manager) {

    std::string path_string = file_path.string();

    JSON_MaterialDefinition document;
    json5::error result = json5::from_file(path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::make_expected<Material, lib::Result<MaterialError>>(
            lib::Result<MaterialError> { .errc = MaterialError::IO, .message = "Could not open a file" }
        );
    }

    if(result != json5::error::none) {
        return lib::make_expected<Material, lib::Result<MaterialError>>(
            lib::Result<MaterialError> { .errc = MaterialError::ParseError, .message = "Parse file error" }
        );
    }

    std::unordered_map<std::string, AssetPtr<Shader>> passes;

    switch(document.blend_mode) {
        case JSON_MaterialBlendMode::opaque: {
            passes.insert({ "deffered", asset_manager.get_shader("engine/shaders/deffered_pc.shader") });
            passes.insert({ "skybox", asset_manager.get_shader("engine/shaders/skybox.shader") });
        } break;
        case JSON_MaterialBlendMode::translucent: {
            passes.insert({ "forward", asset_manager.get_shader("engine/shaders/forward_pc.shader") });
        } break;
    }

    if(document.passes.has_value()) {
        for(auto const& pass : document.passes.value()) {
            passes.at(pass.name) = asset_manager.get_shader(pass.shader);
        }
    }

    std::unordered_map<std::string, MaterialParameter> valid_shader_parameters;

    for(auto const& [pass_name, pass_shader] : passes) {
        
        if(pass_shader->is_pending()) {
            pass_shader->wait();
        }

        if(pass_shader->is_ok()) {

            for(auto const& uniform : pass_shader->get().variants.at(1 << 0).uniforms) {

                if(!uniform.name.contains("material") && uniform.is_buffer()) {
                    continue;
                }
                        
                auto uniform_visitor = make_visitor(
                    [&](ShaderUniformData<ShaderUniformType::Sampler2D> const&) {
                        valid_shader_parameters.insert_or_assign(uniform.name, MaterialParameter { .data = MaterialParameterData<MaterialParameterType::Sampler2D> { .value = nullptr } });
                    },
                    [&](ShaderUniformData<ShaderUniformType::SamplerCube> const&) {
                        valid_shader_parameters.insert_or_assign(uniform.name, MaterialParameter { .data = MaterialParameterData<MaterialParameterType::SamplerCube> { .value = nullptr } });
                    },
                    [&](ShaderUniformData<ShaderUniformType::CBuffer> const& data) {

                        for(auto const& variable : data.data) {

                            switch(variable.type) {
                                case ShaderDataType::F32: {
                                    valid_shader_parameters.insert_or_assign(uniform.name, MaterialParameter { .data = MaterialParameterData<MaterialParameterType::F32> {} });
                                } break;

                                case ShaderDataType::F32x2: {
                                    valid_shader_parameters.insert_or_assign(uniform.name, MaterialParameter { .data = MaterialParameterData<MaterialParameterType::F32x2> {} });
                                } break;

                                case ShaderDataType::F32x3: {
                                    valid_shader_parameters.insert_or_assign(uniform.name, MaterialParameter { .data = MaterialParameterData<MaterialParameterType::F32x3> {} });
                                } break;

                                case ShaderDataType::F32x4: {
                                    valid_shader_parameters.insert_or_assign(uniform.name, MaterialParameter { .data = MaterialParameterData<MaterialParameterType::F32x4> {} });
                                } break;

                                case ShaderDataType::F32x4x4: {
                                    valid_shader_parameters.insert_or_assign(uniform.name, MaterialParameter { .data = MaterialParameterData<MaterialParameterType::F32x4x4> {} });
                                } break;
                            }
                        }
                    },
                    [&](ShaderUniformData<ShaderUniformType::RWTexture2D> const&) { },
                    [&](ShaderUniformData<ShaderUniformType::SBuffer> const&) { },
                    [&](ShaderUniformData<ShaderUniformType::RWBuffer> const&) { }
                );

                std::visit(uniform_visitor, uniform.data);
            }

        } else {
            return lib::make_expected<Material, lib::Result<MaterialError>>(
                lib::Result<MaterialError> { .errc = MaterialError::InvalidShader, .message = "Invalid shader error" }
            );
        }
    }

    std::unordered_map<std::string, MaterialParameter> parameters;

    for(auto const& parameter : document.parameters) {

        auto it = valid_shader_parameters.find(parameter.name);

        if(it == valid_shader_parameters.end()) {
            return lib::make_expected<Material, lib::Result<MaterialError>>(
                lib::Result<MaterialError> { .errc = MaterialError::InvalidParameter, .message = "Invalid parameter error" }
            );
        }

        if(parameter.type == JSON_MaterialParameterType::sampler2D) {

            if(parameter.value.path.has_value()) {
                it->second.as_sampler2D().value = asset_manager.get_texture(parameter.value.path.value());
                parameters.insert({ parameter.name, std::move(it->second) });
            } else {
                return lib::make_expected<Material, lib::Result<MaterialError>>(
                    lib::Result<MaterialError> { .errc = MaterialError::InvalidParameter, .message = "Invalid value error" }
                );
            }

        } else if(parameter.type == JSON_MaterialParameterType::samplerCube) {

            if(parameter.value.path.has_value()) {
                it->second.as_samplerCube().value = asset_manager.get_texture(parameter.value.path.value());
                parameters.insert({ parameter.name, std::move(it->second) });
            } else {
                return lib::make_expected<Material, lib::Result<MaterialError>>(
                    lib::Result<MaterialError> { .errc = MaterialError::InvalidParameter, .message = "Invalid value error" }
                );
            }
        } else {
            switch(parameter.type) {
                case JSON_MaterialParameterType::f32: {
                    auto& value = parameter.value.f32.value();
                    it->second.as_f32().value = value;
                    parameters.insert({ parameter.name, std::move(it->second) });
                } break;

                case JSON_MaterialParameterType::f32x2: {
                    auto& value = parameter.value.f32x2.value();
                    it->second.as_f32x2().value = lib::math::Vector2f(value.at(0), value.at(1));
                    parameters.insert({ parameter.name, std::move(it->second) });
                } break;

                case JSON_MaterialParameterType::f32x3: {
                    auto& value = parameter.value.f32x3.value();
                    it->second.as_f32x3().value = lib::math::Vector3f(value.at(0), value.at(1), value.at(2));
                    parameters.insert({ parameter.name, std::move(it->second) });
                } break;

                case JSON_MaterialParameterType::f32x4: {
                    auto& value = parameter.value.f32x4.value();
                    it->second.as_f32x4().value = lib::math::Vector4f(value.at(0), value.at(1), value.at(2), value.at(3));
                    parameters.insert({ parameter.name, std::move(it->second) });
                } break;

                case JSON_MaterialParameterType::f32x4x4: {
                    
                } break;

                default: {
                    assert(false && "invalid data type");
                } break;
            }
        }
    }

    auto material = Material {
        .name = document.name,
        .domain = get_material_domain(document.domain),
        .blend_mode = get_material_blend_mode(document.blend_mode),
        .parameters = std::move(parameters),
        .passes = std::move(passes),
        .hash = XXHash64::hash(reinterpret_cast<void*>(document.name.data()), document.name.size(), 0)
    };

    return lib::make_expected<Material, lib::Result<MaterialError>>(std::move(material));
}

MaterialDomain constexpr ionengine::asset::get_material_domain(JSON_MaterialDomain const domain) {
    switch(domain) {
        case JSON_MaterialDomain::surface: return MaterialDomain::Surface;
        default: {
            assert(false && "invalid data type");
            return MaterialDomain::Surface;
        }
    }
}

MaterialBlendMode constexpr ionengine::asset::get_material_blend_mode(JSON_MaterialBlendMode const blend_mode) {
    switch(blend_mode) {
        case JSON_MaterialBlendMode::opaque: return MaterialBlendMode::Opaque;
        case JSON_MaterialBlendMode::translucent: return MaterialBlendMode::Translucent;
        default: {
            assert(false && "invalid data type");
            return MaterialBlendMode::Opaque;
        }
    }
}
