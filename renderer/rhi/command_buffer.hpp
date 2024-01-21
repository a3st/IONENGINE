// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "math/color.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

enum class RenderPassLoadOp {
    Load,
    Clear,
    DontCare
};

enum class RenderPassStoreOp {
    Store,
    DontCare
};

struct RenderPassColorInfo {
    core::ref_ptr<Texture> texture;
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
    math::Color clear_color;
};

struct RenderPassDepthStencilInfo {
    core::ref_ptr<Texture> texture;
    RenderPassLoadOp depth_load_op;
    RenderPassStoreOp depth_store_op;
    RenderPassLoadOp stencil_load_op;
    RenderPassStoreOp stencil_store_op;
    float const clear_depth;
    uint8_t const clear_stencil;
}

enum class IndexFormat {
    Uint16,
    Uint32
};

class CommandBuffer : public core::ref_counted_object {
public:

    virtual auto begin_render_pass(
        std::span<RenderPassColorInfo const> const colors,
        std::optional<RenderPassDepthStencilInfo> depth_stencil
    ) = 0;

    virtual auto end_render_pass() -> void = 0;

    virtual auto bind_vertex_buffer(uint32_t const slot, core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void;

    virtual auto bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size, IndexFormat const format) -> void;

    virtual auto draw_indexed(uint32_t const index_count, uint32_t const instance_count, uint32_t instance_offset) -> void;
};

}

}

}