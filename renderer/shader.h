// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

template<class Type>
struct ShaderInput {
    std::u8string name;
};

template<>
struct ShaderInput<float> {
    std::u8string name;
    float value;
    std::pair<float, float> min_max;
};

template<>
struct ShaderInput<Vector3f> {
    std::u8string name;
    Vector3f value;
};

template<>
struct ShaderInput<Vector4f> {
    std::u8string name;
    Vector4f value;
};

template<>
struct ShaderInput<Vector2f> {
    std::u8string name;
    Vector2f value;
};

template<>
struct ShaderInput<Sampler> {
    std::u8string name;
};

using ShaderInputDesc = std::variant<ShaderInput<float>, ShaderInput<Vector3f>, ShaderInput<Vector4f>, ShaderInput<Vector2f>, ShaderInput<Sampler>>;

using ShaderEffectId = uint32_t;

struct ShaderEffectDesc {
    std::map<std::u8string, ShaderPackageData::ShaderInfo const*> shader_infos;

    ShaderEffectDesc& set_shader_code(std::u8string const& name, ShaderPackageData::ShaderInfo const& shader_info, ShaderFlags const flags) {

        shader_infos[name] = &shader_info;
        return *this;
    }
};

struct ShaderEffect {

    std::unordered_map<ShaderFlags, Handle<Shader>> shaders;
};

class ShaderEffectBinder {
public:

    ShaderEffectBinder(ShaderEffect& shader_effect);

private:

    ShaderEffect* _shader_effect;
};

class ShaderCache {
public:

    ShaderCache() = default;

    void create_shader_effect(Backend& backend, ShaderEffectId const id, ShaderEffectDesc const& shader_effect_desc);

    ShaderEffect const& get_shader_effect(ShaderEffectId const id) const;

private:

    std::unordered_map<ShaderEffectId, ShaderEffect> _shader_effects;
    std::map<std::u8string, Handle<Shader>> _shader_cache;
};
    
}


/*
struct ShaderTemplate {
    ShaderDomain domain;
    ShaderBlendMode blend_mode;
    CullMode cull_mode;
    FillMode fill_mode;

    struct PassData {
        std::unordered_map<ShaderFlags, Handle<Shader>> shaders;
    };

    std::array<std::vector<PassData>, 2> pass_data;
};*/

/*
struct ShaderBuildDesc {
    ShaderDomain domain;
    ShaderBlendMode blend_mode;
    CullMode cull_mode;
    FillMode fill_mode;
};

struct ShaderPassDesc {
    uint32_t pass_index;
    ShaderQuality shader_quality;

    

    ShaderPassDesc& set_pass_index(uint32_t const index) {

        pass_index = index;
        return *this;
    }

    ShaderPassDesc& set_quality(ShaderQuality const quality) {

        shader_quality = quality;
        return *this;
    }

    
};*/

/*
enum class ShaderDomain {
    Surface,
    Decal,
    PostProcess
};

enum class ShaderBlendMode {
    Opaque,
    Transparent
};

enum class ShaderQuality {
    Low,
    High
};
*/