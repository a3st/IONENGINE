// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/resource_ptr.h>

namespace ionengine::renderer {

struct GPUProgram;
struct GPUBuffer;
struct GPUTexture;
struct CommandList;

struct NullData {
    ResourcePtr<GPUBuffer> cbuffer;
    ResourcePtr<GPUBuffer> sbuffer;
    ResourcePtr<GPUTexture> texture;
};

class DescriptorBinder {
public:

    DescriptorBinder(ResourcePtr<GPUProgram> program, NullData& null_data);

    DescriptorBinder(DescriptorBinder&) = delete;

    DescriptorBinder(DescriptorBinder&&) noexcept = delete;

    DescriptorBinder& operator=(DescriptorBinder&) = delete;

    DescriptorBinder& operator=(DescriptorBinder&&) noexcept = delete;

    void update(uint32_t const index, GPUTexture const& texture);

    void update(uint32_t const index, GPUBuffer const& buffer);

    void bind(backend::Device& device, CommandList& command_list);

private:

    ResourcePtr<GPUProgram> _program;

    std::unordered_map<uint32_t, size_t> _exist_updates;
    std::vector<backend::DescriptorWriteDesc> _update_resources;
};

}
