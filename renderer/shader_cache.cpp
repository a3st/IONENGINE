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

        auto it = _pass_hashes.find(shader.name);

        if(it != _pass_hashes.end()) {
            lib::logger().warning(lib::LoggerCategoryType::Renderer, std::format("shader with '{}' name is already loaded", shader.name));
            return;
        } else {

            std::string const pass_name = shader.name;
            uint64_t const hash = shader.hash;

            auto cache_entry = CacheEntry<ResourcePtr<Shader>> {
                .value = make_resource_ptr(std::move(shader)),
                .hash = hash
            };

            size_t const cache_entry_value = _data.push(std::move(cache_entry));

            _pass_hashes.insert({ std::move(pass_name), cache_entry_value });
        }

    } else {
        throw lib::Exception(result.as_error().message);
    }
}

ResourcePtr<Shader> ShaderCache::get(asset::AssetPtr<asset::Material> material, std::string_view const pass_name) {
    std::string pass_name_string = std::string(pass_name.begin(), pass_name.end());
    uint64_t hash = std::numeric_limits<uint64_t>::max();
    {
        std::shared_lock lock(_mutex);

        auto it = material->get().cache_entries.find(pass_name_string);

        if(it != material->get().cache_entries.end()) {
            hash = it->second; 
        }
    }

    if(hash == std::numeric_limits<uint64_t>::max()) {
        std::lock_guard lock(_mutex);

        auto it = _pass_hashes.find(pass_name_string);

        if(it != _pass_hashes.end()) {
            hash = it->second;
        }

        material->get().cache_entries.insert({ pass_name_string, hash });
    }

    std::shared_lock lock(_mutex);
    if(_data.is_valid(hash)) {
        return _data.get(hash).value;
    }
    return nullptr;
}

void ShaderCache::update(float const delta_time) {

}
