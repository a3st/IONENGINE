// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/context.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

Context::Context(platform::Window& window, uint32_t const buffered_frame_count) : 
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = buffered_frame_count }),
    _buffered_frame_count(buffered_frame_count) {

    _graphics_fence_values.resize(buffered_frame_count);


    /*std::generate_n(
        _upload_buffers.begin(), 
        buffered_frame_count, 
        [&] { 
            return UploadBuffer {
                .command_list = _device.create_command_list(backend::QueueFlags::Copy),
                .buffer = _device.create_buffer(32 * 1024 * 1024, backend::BufferFlags::HostWrite),
                .buffer_size = 32 * 1024 * 1024,
                .offset = 0
            };
        }
    );*/
}

void Context::swap_buffers() {

    _device.present();
    _frame_index = (_frame_index + 1) % _buffered_frame_count;
}

backend::Handle<backend::Buffer> Context::create_vertex_buffer(uint32_t const vertices_count) {

    uint32_t const buffer_size = vertices_count * sizeof(float);
    return _device.create_buffer(buffer_size, backend::BufferFlags::VertexBuffer);
}

backend::Handle<backend::Buffer> Context::create_index_buffer(uint32_t const indices_count) {

    uint32_t const buffer_size = indices_count * sizeof(uint32_t);
    assert(buffer_size <= 65536);
    return _device.create_buffer(buffer_size, backend::BufferFlags::IndexBuffer);
}

backend::Handle<backend::Texture> Context::get_or_wait_previous_frame() {

    backend::Handle<backend::Texture> next_texture = _device.acquire_next_texture();
    _device.wait(_graphics_fence_values[_frame_index], backend::QueueFlags::Graphics);
    return next_texture;
}

void Context::upload_buffer_data(backend::Handle<backend::Buffer> const& dest, uint64_t const offset, std::span<uint8_t const> const data, BufferUsage const usage) {

    if(usage == BufferUsage::Dynamic) {

        _device.map_buffer_data(dest, offset, data);
    } else {

        auto& upload_buffer = _upload_buffers.at(_frame_index);

        if(upload_buffer.offset + data.size() > upload_buffer.buffer_size) {
            wait_upload_data();
        }

        _device.map_buffer_data(upload_buffer.buffer, upload_buffer.offset, data);

        _device.barrier(upload_buffer.command_list, dest, backend::MemoryState::Common, backend::MemoryState::CopyDest);
        _device.barrier(upload_buffer.command_list, upload_buffer.buffer, backend::MemoryState::Common, backend::MemoryState::CopySource);
        _device.copy_buffer_region(
            upload_buffer.command_list,
            dest,
            offset,
            upload_buffer.buffer,
            upload_buffer.offset,
            data.size()
        );
        _device.barrier(upload_buffer.command_list, dest, backend::MemoryState::CopyDest, backend::MemoryState::Common);
        _device.barrier(upload_buffer.command_list, upload_buffer.buffer, backend::MemoryState::CopySource, backend::MemoryState::Common);
    
        upload_buffer.offset += data.size();
    }
}

void Context::submit_or_skip_upload_data() {

    auto& upload_buffer = _upload_buffers.at(_frame_index);

    if(upload_buffer.offset > 0) {

        _copy_fence_value = _device.submit(
            std::span<backend::Handle<backend::CommandList> const>(&upload_buffer.command_list, 1),
            backend::QueueFlags::Copy
        );
    }
}

void Context::wait_upload_data() {

    _device.wait(_copy_fence_value, backend::QueueFlags::Copy);
}

backend::Device& Context::device() {

    return _device;
}

uint64_t Context::graphics_fence_value() const {

    return _graphics_fence_values[_frame_index];
}

void Context::graphics_fence_value(uint64_t const value) {

    _graphics_fence_values[_frame_index] = value;
}
