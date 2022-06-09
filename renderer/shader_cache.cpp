// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_cache.h>

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

lib::ObjectPtr<Shader> ShaderCache::get(std::string_view const shader_name, std::optional<std::filesystem::path> const shader_path) {

    std::unique_lock lock(_mutex);

    auto it = _data.find(std::string(shader_name.begin(), shader_name.end()));

    if(it != _data.end()) {

        if(shader_path.has_value()) {

            auto result = Shader::load_from_file(shader_path.value(), *_device);

            if(result.is_ok()) {

                Shader shader = std::move(result.value());
            
                if(shader.hash != it->second.hash) {

                    uint64_t const hash = shader.hash;

                    auto cache_entry = CacheEntry<lib::ObjectPtr<Shader>> {
                        .value = std::move(shader),
                        .hash = hash
                    };

                    it->second = std::move(cache_entry);
                }
            } else {
                throw lib::Exception(result.error_value().message);
            }
        } else {
            return it->second.value;
        }
    } else {

        auto result = Shader::load_from_file(shader_path.value(), *_device);
        
        if(result.is_ok()) {

            Shader shader = std::move(result.value());
            uint64_t const hash = shader.hash;
            std::string const new_shader_name = shader.name;

            auto cache_entry = CacheEntry<lib::ObjectPtr<Shader>> {
                .value = std::move(shader),
                .hash = hash
            };

            return _data.insert({ new_shader_name, std::move(cache_entry) }).first->second.value;
        } else {
            throw lib::Exception(result.error_value().message);
        }
    }

    return nullptr;
}

void ShaderCache::update(float const delta_time) {

    // for(auto& cache_entry : _data.has_values()) { }
}
