// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/shader.h>
#include <lib/object_ptr.h>

namespace ionengine::renderer {

class ShaderCache {
public:

    ShaderCache(backend::Device& device);

    ShaderCache(ShaderCache const&) = delete;

    ShaderCache(ShaderCache&& other) noexcept;

    ShaderCache& operator=(ShaderCache const&) = delete;

    ShaderCache& operator=(ShaderCache&& other) noexcept;

    lib::ObjectPtr<Shader> get(std::string_view const shader_name, std::optional<std::filesystem::path> const shader_path = std::nullopt);

    void update(float const delta_time);

private:

    backend::Device* _device;

    std::unordered_map<std::string, CacheEntry<lib::ObjectPtr<Shader>>> _data;
};

}
