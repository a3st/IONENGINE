// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>

namespace ionengine::renderer {

class UploadContext {
public:

    UploadContext(backend::Device& device);

    void begin();

    void copy_buffer_data(backend::Handle<backend::Buffer> const& dest, uint64_t const offset, std::span<uint8_t const> const data);

    void end();

private:

    backend::Device* _device;

    backend::Handle<backend::CommandList> _command_list;

    backend::Handle<backend::Buffer> _buffer;
    uint64_t _offset{0};
};

}