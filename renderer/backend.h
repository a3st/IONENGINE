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

enum class RenderPassLoadOp {
    Load,
    Clear,
    DontCare
};

enum class RenderPassStoreOp {
    Store,
    DontCare
};

enum class Format {
    Unknown,
    RGBA32float,
    RGBA32uint,
    RGBA32int,
    RGB32float,
    RGB32uint,
    RGB32int,
    RG32float,
    RG32uint,
    RG32int,
    R32float,
    R32uint,
    R32int,
    RGBA16float,
    RGBA16uint,
    RGBA16int,
    RGBA16unorm,
    RGBA16snorm,
    RG16float,
    RG16uint,
    RG16int,
    RG16unorm,
    RG16snorm,
    R16float,
    R16uint,
    R16int,
    R16unorm,
    R16snorm,
    RGBA8uint,
    RGBA8int,
    RGBA8unorm,
    RGBA8snorm,
    RG8uint,
    RG8int,
    RG8unorm,
    RG8snorm,
    R8uint,
    R8int,
    R8unorm,
    R8snorm
};

struct RenderPassColorDesc {
    Format format = Format::Unknown;
    RenderPassLoadOp load_op = RenderPassLoadOp::DontCare;
    RenderPassStoreOp store_op = RenderPassStoreOp::DontCare;
};

struct RenderPassDepthStencilDesc {
    Format format = Format::Unknown;
    RenderPassLoadOp depth_load_op = RenderPassLoadOp::DontCare;
    RenderPassStoreOp depth_store_op = RenderPassStoreOp::DontCare;
    RenderPassLoadOp stencil_load_op = RenderPassLoadOp::DontCare;
    RenderPassStoreOp stencil_store_op = RenderPassStoreOp::DontCare;
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
    friend class CommandBuffer;
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

class RenderPass {
public:

    RenderPass();

    RenderPass(
        Backend& backend, 
        std::span<RenderPassColorDesc const> const colors, 
        std::optional<RenderPassDepthStencilDesc> const depth_stencil, 
        uint16_t const sample_count
    );

    ~RenderPass();


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

    void reset();

    void close();

    void begin_render_pass();

    void end_render_pass();

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}