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

    friend class Context;

public:

    UploadBuffer(backend::Device& device, UploadBufferFlags const flags);

    UploadBuffer(UploadBuffer const& other) = delete;

    UploadBuffer(UploadBuffer&& other) noexcept;

    UploadBuffer& operator=(UploadBuffer&& other) noexcept;

    UploadBuffer& operator=(UploadBuffer const& other) = delete;

    ~UploadBuffer();

    void upload_data(backend::Handle<backend::Buffer> const dest, uint64_t const offset, std::span<uint8_t const> const data);

private:

    backend::Device* _device{nullptr};

    backend::Handle<backend::Buffer> _upload_buffer{backend::InvalidHandle<backend::Buffer>()};
    backend::Handle<backend::CommandList> _command_list{backend::InvalidHandle<backend::CommandList>()};

    uint64_t _offset{0};
};

}