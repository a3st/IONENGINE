// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>

namespace ionengine::platform {
class Window;
}

namespace ionengine::renderer::frontend {

enum class BufferUsage {
    Static,
    Dynamic
};

class Context {
public:

    Context(platform::Window& window, uint32_t const buffered_frame_count);

    Context(Context const&) = delete;

    Context(Context&&) noexcept = delete;

    Context& operator=(Context const&) = delete;

    Context& operator=(Context&&) noexcept = delete;

    void swap_buffers();

    backend::Device& device();

    backend::Handle<backend::Texture> get_or_wait_previous_frame();

    void upload_buffer_data(backend::Handle<backend::Buffer> const& dest, uint64_t const offset, std::span<uint8_t const> const data, BufferUsage const usage);

    void submit_or_skip_upload_buffers();

    uint64_t graphics_fence_value() const;

    void graphics_fence_value(uint64_t const value);

private:

    backend::Device _device;

    uint32_t _buffered_frame_count{0};
    uint32_t _frame_index{0};

    std::vector<uint64_t> _graphics_fence_values;

    uint64_t _copy_fence_value{0};
};

}
