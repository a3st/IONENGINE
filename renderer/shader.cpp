// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader.h>

using namespace ionengine::renderer;

/*void ShaderManager::build_shader_template(
    Backend& backend, 
    ShaderTemplateId const id, 
    std::span<ShaderPassDesc const> const shader_pass_descs, 
    ShaderBuildDesc const shader_build_desc
) {

    std::map<std::pair<uint32_t, ShaderQuality>, uint32_t> shader_passes_by_quality;
    uint32_t shader_pass_count = 0;

    for(size_t i = 0; i < shader_pass_descs.size(); ++i) {
        shader_passes_by_quality[{ shader_pass_descs[i].pass_index , shader_pass_descs[i].shader_quality }] = static_cast<uint32_t>(i);
        shader_pass_count = std::max<uint32_t>(shader_pass_count, shader_pass_descs[i].pass_index);
    }
    ++shader_pass_count;

    auto shader_template = ShaderTemplate {};
    shader_template.domain = shader_build_desc.domain;
    shader_template.blend_mode = shader_build_desc.blend_mode;
    shader_template.fill_mode = shader_build_desc.fill_mode;
    shader_template.cull_mode = shader_build_desc.cull_mode;
    shader_template.pass_data[static_cast<uint32_t>(ShaderQuality::Low)].resize(shader_pass_count);
    shader_template.pass_data[static_cast<uint32_t>(ShaderQuality::High)].resize(shader_pass_count);

    for(auto const& [key, value] : shader_passes_by_quality) {

        for(auto const& [name, shader_info] : shader_pass_descs[value].shader_infos) {

            auto it = _shader_cache.find(name);

            if(it != _shader_cache.end()) {
                shader_template.pass_data[static_cast<uint32_t>(key.second)][key.first].shaders[shader_info->flags] = it->second;
            } else {
                Handle<Shader> shader = backend.create_shader(shader_info->data, shader_info->flags);
                shader_template.pass_data[static_cast<uint32_t>(key.second)][key.first].shaders[shader_info->flags] = shader;
                _shader_cache[name] = shader;
            }
        }
    }
}

ShaderTemplate const& ShaderManager::get_shader_template(ShaderTemplateId const id) const {

    return _shader_templates.at(id);
}*/

void ShaderCache::create_shader_effect(Backend& backend, ShaderEffectId const id, ShaderEffectDesc const& shader_effect_desc) {

    auto shader_effect = ShaderEffect {};

    for(auto const& [name, shader_info] : shader_effect_desc.shader_infos) {

        auto it = _shader_cache.find(name);
        if(it != _shader_cache.end()) {
            shader_effect.shaders[shader_info->flags] = it->second;
        } else {
            Handle<Shader> shader = backend.create_shader(shader_info->data, shader_info->flags);
            shader_effect.shaders[shader_info->flags] = shader;
            _shader_cache[name] = shader;
        }
    }

    _shader_effects[id] = std::move(shader_effect);
}

ShaderEffect const& ShaderCache::get_shader_effect(ShaderEffectId const id) const {

    return _shader_effects.at(id);
}