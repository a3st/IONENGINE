// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/resource_ptr.h>
#include <renderer/gpu_buffer.h>
#include <renderer/gpu_texture.h>

namespace ionengine::renderer {

struct Shader;

struct NullData {
    ResourcePtr<GPUBuffer> cbuffer;
    ResourcePtr<GPUBuffer> sbuffer;
    ResourcePtr<GPUTexture> texture;
};

class ShaderBinder {
public:

    ShaderBinder(Shader& shader, NullData& null);

    ShaderBinder(ShaderBinder&) = delete;

    ShaderBinder(ShaderBinder&&) noexcept = delete;

    ShaderBinder& operator=(ShaderBinder&) = delete;

    ShaderBinder& operator=(ShaderBinder&&) noexcept = delete;

    void update_resource(uint32_t const location, backend::ResourceHandle resource);

    void bind(backend::Device& device, backend::Handle<backend::CommandList> const& command_list);

private:

    backend::Handle<backend::DescriptorLayout> _descriptor_layout;
    std::unordered_map<uint32_t, size_t> _exist_updates;
    std::vector<backend::DescriptorWriteDesc> _update_resources;
};

}
