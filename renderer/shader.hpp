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
    Array,
    Vector,
    Matrix,
    Sampler,
    SamplerComparison,
    Texture1D,
    Texture2D,
    Texture2DArray,
    Texture3D,
    TextureCube,
    TextureCubeArray,
    TextureMultisampled2D,
    TextureDepthMultisampled2D,
    TextureDepth2D,
    TextureDepth2DArray,
    TextureDepthCube,
    TextureDepthCubeArray,
    TextureStorage1D,
    TextureStorage2D,
    TextureStorage2DArray,
    TextureStorage3D
};

enum class VariableFormat {
    Undefined,
    Float32,
    Uint32,
    Sint32
};

struct VariableInfo {
    VariableType variable_type;
    VariableFormat format;
};

struct VariableReflectInfo {
    std::string name;
    VariableInfo variable_info;
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

    auto get_stages() -> wgpu::ShaderStage const& {

        return stages;
    }

private:

    const uint32_t MAX_GROUP_TYPE_SPACE = 6;
    const uint32_t MAX_GROUP_HANDLE_SPACE = 4;

    wgpu::ShaderStage stages{wgpu::ShaderStage::None};
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, VariableReflectInfo>> bindings;

    auto get_format_by_string(std::string_view const format) -> VariableFormat;

    auto get_access_by_string(std::string_view const access) -> VariableAccessType;

    auto parse_variable_type(std::string const& source, VariableReflectInfo& reflect_info) -> void;

    auto parse_resource_type(std::tuple<std::string, std::string> const sources, VariableReflectInfo& reflect_info) -> void;
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