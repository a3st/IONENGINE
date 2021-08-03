// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Instance;
class Adapter;
class Device;
class CommandQueue;
class Swapchain;
class Shader;
class Pipeline;
class DescriptorSetLayout;

enum class Format;

namespace utils {

    std::vector<byte> read_shader_code(const std::filesystem::path& shader_path) {

        std::vector<byte> shader_code;

        std::ifstream file(shader_path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error(format<std::string>("Shader cannot be opened '{}'", shader_path));
        }

        usize file_size = static_cast<usize>(file.tellg());
        shader_code.resize(file_size);

        file.seekg(std::ios::beg);
        file.read(reinterpret_cast<char*>(shader_code.data()), file_size);
        return std::move(shader_code);
    }
}

enum class ShaderType {
    Vertex,
    Pixel,
    Geometry,
    Compute,
    Hull,
    Domain,
    All
};

enum class ViewDimension {
    Unknown,
    Buffer,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture3D,
    TextureCube,
    TextureCubeArray
};

enum class ViewType {
    Unknown,
    ConstantBuffer,
    Sampler,
    Texture,
    RWTexture,
    Buffer,
    RWBuffer,
    StructuredBuffer,
    RWStructuredBuffer
};

enum class CommandListType {
    Graphics,
    Copy,
    Compute
};

enum class TextureType {
    _1D,
    _2D,
    _3D
};

enum class FillMode {
    Wireframe,
    Solid
};

enum class CullMode {
    None,
    Front,
    Back
};

struct InputLayoutDesc {
    uint32 slot;
    std::string semantic_name;
    Format format;
    uint32 stride;
};

struct DescriptorSetLayoutBinding {
    ShaderType shader_type;
    ViewType view_type;
    uint32 slot;
    uint32 space;
    uint32 count;
};

struct GraphicsPipelineDesc {


    std::vector<InputLayoutDesc> input;
};

}