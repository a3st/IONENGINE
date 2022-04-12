// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/upload_buffer.h>

namespace ionengine::platform {
class Window;
}

namespace ionengine::renderer {

class Context {
public:

    Context(platform::Window& window, uint32_t const buffered_frame_count);

    Context(Context const&) = delete;

    Context(Context&&) noexcept = delete;

    Context& operator=(Context const&) = delete;

    Context& operator=(Context&&) noexcept = delete;

    uint64_t graphics_fence_value() const { return _graphics_fence_values[_frame_index]; }

    void graphics_fence_value(uint64_t const value) { _graphics_fence_values[_frame_index] = value; }

    void swap_buffers();

    uint32_t buffered_frame_count() const { return _buffered_frame_count; }

    backend::Device& device() { return _device; }

    backend::Handle<backend::Texture> get_or_wait_previous_frame() {

        backend::Handle<backend::Texture> swapchain_texture = _device.acquire_next_texture();
        _device.wait(_graphics_fence_values[_frame_index], backend::QueueFlags::Graphics);
        return swapchain_texture;
    }

    UploadBuffer& upload_buffer(UploadBufferFlags const flags);

    void submit_upload_buffers();

private:

    backend::Device _device;

    std::vector<uint64_t> _graphics_fence_values;

    uint32_t _buffered_frame_count{0};
    uint32_t _frame_index{0};

    
    std::optional<UploadBuffer> _sync_upload_buffer;
    std::vector<UploadBuffer> _async_upload_buffers;
};

}
