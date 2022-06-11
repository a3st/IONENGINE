// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/material.h>
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

    auto material = Material {};
    {
        material.name = document.name;
        material.domain = get_material_domain(document.domain);
        material.blend_mode = get_material_blend_mode(document.blend_mode);

        for(auto const& parameter : document.parameters) {

            if(parameter.type == JSON_MaterialParameterType::sampler2D) {
                material.parameters.insert(
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
                        material.parameters.insert(
                            {
                                parameter.name,
                                MaterialParameter {
                                    .data = MaterialParameterData<asset::MaterialParameterType::F32> {
                                        .value = value
                                    }
                                }
                            }
                        );
                    } break;
                    case JSON_MaterialParameterType::f32x2: {
                        auto value = parameter.value.vec2.value();
                        material.parameters.insert(
                            {
                                parameter.name,
                                MaterialParameter {
                                    .data = MaterialParameterData<asset::MaterialParameterType::F32x2> {
                                        .value = lib::math::Vector2f(value.at(0), value.at(1))
                                    }
                                }
                            }
                        );
                    } break;
                    case JSON_MaterialParameterType::f32x3: {
                        auto value = parameter.value.vec3.value();
                        material.parameters.insert(
                            {
                                parameter.name,
                                MaterialParameter {
                                    .data = MaterialParameterData<asset::MaterialParameterType::F32x3> {
                                        .value = lib::math::Vector3f(value.at(0), value.at(1), value.at(2))
                                    }
                                }
                            }
                        );
                    } break;
                    case JSON_MaterialParameterType::f32x4: {
                        auto value = parameter.value.vec4.value();
                        material.parameters.insert(
                            {
                                parameter.name,
                                MaterialParameter {
                                    .data = MaterialParameterData<asset::MaterialParameterType::F32x4> {
                                        .value = lib::math::Vector4f(value.at(0), value.at(1), value.at(2), value.at(3))
                                    }
                                }
                            }
                        );
                    } break;

                    default: {
                        assert(false && "invalid data type");
                    } break;
                }
            }
        }

        for(auto const& pass : document.passes) {
            material.passes.insert({ pass.name, pass.shader });
        }

        material.hash = XXHash64::hash(reinterpret_cast<void*>(material.name.data()), material.name.size(), 0);
    }
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
        case JSON_MaterialBlendMode::transculent: return MaterialBlendMode::Transculent;
        default: {
            assert(false && "invalid data type");
            return MaterialBlendMode::Opaque;
        }
    }
}
