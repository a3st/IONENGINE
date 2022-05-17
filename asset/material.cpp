// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/material.h>
#include <lib/exception.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::asset;

Material::Material(JSON_MaterialDefinition const& document, AssetManager& asset_manager) {

    _name = document.name;

    for(auto const& parameter : document.parameters) {

        if(parameter.type == JSON_MaterialParameterType::sampler2D) {
            _parameters.insert(
                { 
                    parameter.name, 
                    MaterialParameter { 
                        .data = MaterialParameterData<asset::MaterialParameterType::Sampler2D> { 
                            .asset = asset_manager.get_texture(parameter.value.path.value())
                        } 
                    } 
                }
            );
        } else {
            switch(parameter.type) {
                case JSON_MaterialParameterType::f32: {
                    auto value = parameter.value.value.value();
                    _parameters.insert(
                        {
                            parameter.name,
                            MaterialParameter {
                                .data = MaterialParameterData<asset::MaterialParameterType::F32> {
                                    .value = value,
                                    .uniform = parameter.value.uniform.value(),
                                    .offset = static_cast<uint64_t>(parameter.value.offset.value())
                                }
                            }
                        }
                    );
                } break;
                case JSON_MaterialParameterType::f32x2: {
                    auto value = parameter.value.vec2.value();
                    _parameters.insert(
                        {
                            parameter.name,
                            MaterialParameter {
                                .data = MaterialParameterData<asset::MaterialParameterType::F32x2> {
                                    .value = lib::math::Vector2f(value.at(0), value.at(1)),
                                    .uniform = parameter.value.uniform.value(),
                                    .offset = static_cast<uint64_t>(parameter.value.offset.value())
                                }
                            }
                        }
                    );
                } break;
                case JSON_MaterialParameterType::f32x3: {
                    auto value = parameter.value.vec3.value();
                    _parameters.insert(
                        {
                            parameter.name,
                            MaterialParameter {
                                .data = MaterialParameterData<asset::MaterialParameterType::F32x3> {
                                    .value = lib::math::Vector3f(value.at(0), value.at(1), value.at(2)),
                                    .uniform = parameter.value.uniform.value(),
                                    .offset = static_cast<uint64_t>(parameter.value.offset.value())
                                }
                            }
                        }
                    );
                } break;
                case JSON_MaterialParameterType::f32x4: {
                    auto value = parameter.value.vec4.value();
                    _parameters.insert(
                        {
                            parameter.name,
                            MaterialParameter {
                                .data = MaterialParameterData<asset::MaterialParameterType::F32x4> {
                                    .value = lib::math::Vector4f(value.at(0), value.at(1), value.at(2), value.at(3)),
                                    .uniform = parameter.value.uniform.value(),
                                    .offset = static_cast<uint64_t>(parameter.value.offset.value())
                                }
                            }
                        }
                    );
                } break;
            }
        }
    }

    for(auto const& pass : document.passes) {

        _passes.emplace_back(
            pass.name,
            asset_manager.get_technique(pass.technique), 
            MaterialPassParameters {
                .fill_mode = get_pass_fill_mode(pass.parameters.fill_mode),
                .cull_mode = get_pass_cull_mode(pass.parameters.cull_mode)
            }
        );
    }
}

lib::Expected<Material, lib::Result<MaterialError>> Material::load_from_file(std::filesystem::path const& file_path, AssetManager& asset_manager) {

    std::string path_string = file_path.string();

    JSON_MaterialDefinition document;
    json5::error result = json5::from_file(path_string, document);

    if(result == json5::error::could_not_open) {
        return lib::Expected<Material, lib::Result<MaterialError>>::error(
            lib::Result<MaterialError> { .errc = MaterialError::IO, .message = "Could not open a file" }
        );
    }

    if(result != json5::error::none) {
        return lib::Expected<Material, lib::Result<MaterialError>>::error(
            lib::Result<MaterialError> { .errc = MaterialError::ParseError, .message = "Parse file error" }
        );
    }

    return lib::Expected<Material, lib::Result<MaterialError>>::ok(Material(document, asset_manager));
}

std::string_view Material::name() const {
    return _name;
}

std::span<MaterialPass const> Material::passes() const {
    return _passes;
}

std::unordered_map<std::string, MaterialParameter>& Material::parameters() {
    return _parameters;
}

MaterialPassFillMode constexpr Material::get_pass_fill_mode(JSON_PassFillMode const fill_mode) const {
    switch(fill_mode) {
        case JSON_PassFillMode::solid: return MaterialPassFillMode::Solid;
        case JSON_PassFillMode::wireframe: return MaterialPassFillMode::Wireframe;
        default: return MaterialPassFillMode::Solid;
    }
}

MaterialPassCullMode constexpr Material::get_pass_cull_mode(JSON_PassCullMode const cull_mode) const {
    switch(cull_mode) {
        case JSON_PassCullMode::front: return MaterialPassCullMode::Front;
        case JSON_PassCullMode::back: return MaterialPassCullMode::Back;
        case JSON_PassCullMode::none: return MaterialPassCullMode::None;
        default: return MaterialPassCullMode::None;
    }
}
