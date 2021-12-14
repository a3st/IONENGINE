// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform { class Window; }

namespace ionengine::renderer {

enum class MemoryType {
    Default,
    Upload,
    Readback
};

enum class BufferType {
    Vertex,
    Index,
    Constant
};

enum class CommandBufferType {
    Direct,
    Copy,
    Compute
};

class Backend {
public:

    Backend(uint32_t const adapter_index, platform::Window* const window);

    ~Backend();

    Backend(Backend const&) = delete;

    Backend(Backend&&) = delete;

    Backend& operator=(Backend const&) = delete;

    Backend& operator=(Backend&&) = delete;

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;

    friend class Buffer;
    friend class Memory;
};

class Buffer {
public:

    Buffer();

    Buffer(Backend& backend, BufferType const type, size_t const size);

    ~Buffer();

    Buffer(Buffer const&) = delete;

    Buffer(Buffer&&);

    Buffer& operator=(Buffer const&) = delete;

    Buffer& operator=(Buffer&&);

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;

    friend class Memory;
};

class Memory {
public:

    Memory();

    Memory(Backend& backend, MemoryType const type, const size_t size);

    Memory(Memory const&) = delete;

    Memory(Memory&&);

    Memory& operator=(Memory const&) = delete;

    Memory& operator=(Memory&&);

    ~Memory();

    MemoryType get_type() const;

    void bind_buffer(Buffer& buffer, uint64_t offset);

    char8_t* map();

    void unmap();

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class CommandBuffer {
public:

    CommandBuffer();

    CommandBuffer(Backend& backend, CommandBufferType const type);

    CommandBuffer(CommandBuffer const&) = delete;

    CommandBuffer(CommandBuffer&&);

    CommandBuffer& operator=(CommandBuffer const&) = delete;

    CommandBuffer& operator=(CommandBuffer&&);

    ~CommandBuffer();

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}