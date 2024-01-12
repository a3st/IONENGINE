// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <webgpu/webgpu.hpp>
#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

class Backend;

enum class VariableResourceType {
    Uniform,
    Storage,
    Texture,
    Sampler,
    StorageTexture
};

enum class VariableAccessType {
    Undefined,
    Read,
    Write,
    ReadWrite
};

enum class VariableType {
    Struct,
    Template,
    Array,
    Vector,
    Matrix
};

struct VariableReflectInfo {
    std::string name;
    VariableType variable_type;
    uint32_t group;
    uint32_t binding;
    VariableResourceType resource_type;
    VariableAccessType access_type;
};

class ShaderReflect {
public:

    ShaderReflect(std::string_view const shader_code);

    auto get_bindings() -> std::unordered_map<uint32_t, std::unordered_map<uint32_t, VariableReflectInfo>> const& {
        
        return bindings;
    }

private:

    const uint32_t MAX_GROUP_TYPE_SPACE = 6;
    const uint32_t MAX_GROUP_HANDLE_SPACE = 4;

    std::unordered_map<uint32_t, std::unordered_map<uint32_t, VariableReflectInfo>> bindings;
};

class Shader : public core::ref_counted_object {
public:

    Shader(Backend& backend, std::string_view const shader_code);

    auto get_shader_module() const -> wgpu::ShaderModule {

        return shader_module;
    }

    auto get_bind_group_layout() const -> wgpu::BindGroupLayout {

        return bind_group_layout;
    }

private:

    wgpu::ShaderModule shader_module{nullptr};
    wgpu::BindGroupLayout bind_group_layout{nullptr};

    std::unique_ptr<wgpu::CompilationInfoCallback> compilation_callback;
};

}

}