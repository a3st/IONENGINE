// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>

namespace ionengine::renderer {

enum class UploadBufferFlags : uint8_t {
    Sync,
    Async
};

DECLARE_ENUM_CLASS_BIT_FLAG(UploadBufferFlags)

class UploadBuffer {
public:

    UploadBuffer(backend::Handle<backend::CommandList> const& command_list, UploadBufferFlags const flags);

    void upload_data(backend::Device& device, backend::Handle<backend::Buffer> const dest, uint64_t const offset, std::span<uint8_t const> const data);

private:

    backend::Handle<backend::Buffer> _upload_buffer;
    backend::Handle<backend::CommandList> _command_list;

    uint64_t _offset{0};
};

}