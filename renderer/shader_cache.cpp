// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_cache.h>
#include <lib/logger.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderCache::ShaderCache(backend::Device& device) :
    _device(&device) {

}

ShaderCache::ShaderCache(ShaderCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
}

ShaderCache& ShaderCache::operator=(ShaderCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
    return *this;
}

void ShaderCache::cache_shader(std::filesystem::path const shader_path) {
    auto result = Shader::load_from_file(shader_path, *_device);
        
    if(result.is_ok()) {

        Shader shader = std::move(result.as_ok());

        auto it = _data.find(shader.name);

        if(it != _data.end()) {
            lib::logger().warning(lib::LoggerCategoryType::Renderer, std::format("shader with '{}' name is already loaded", shader.name));
            return;
        } else {

            std::string const shader_name = shader.name;
            uint64_t const hash = shader.hash;

            auto cache_entry = CacheEntry<ResourcePtr<Shader>> {
                .value = make_resource_ptr(std::move(shader)),
                .hash = hash
            };

            _data.insert({ std::move(shader_name), cache_entry });
        }

    } else {
        throw lib::Exception(result.as_error().message);
    }
}

ResourcePtr<Shader> ShaderCache::get(asset::AssetPtr<asset::Material> material, std::string_view const pass_shader_name) {
    std::string const pass_name_string = std::string(pass_shader_name);
    std::string shader_name = pass_name_string;
    if(material) {
        auto it = material->get().passes.find(pass_name_string);

        if(it != material->get().passes.end()) {
            shader_name = it->second;
        }
    }

    auto it = _data.find(shader_name);

    if(it != _data.end()) {
        return it->second.value;
    } else {
        throw lib::Exception("");
    }
}

void ShaderCache::update(float const delta_time) {

}
