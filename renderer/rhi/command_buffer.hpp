// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "texture.hpp"
#include "shader.hpp"
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
};

enum class IndexFormat {
    Uint16,
    Uint32
};

enum class FillMode {
    Wireframe,
    Solid
};

enum class CullMode {
    None,
    Front,
    Back
};

enum class CompareOp {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

enum class Blend {
    Zero,
    One,
    SrcAlpha,
    InvSrcAlpha,
    BlendFactor
};

enum class BlendOp {
    Add,
    Subtract,
    RevSubtract,
    Min,
    Max
};

struct DepthStencilStageInfo {
    CompareOp depth_func;
    bool write_enable;
};

struct BlendColorInfo {
    bool blend_enable;
    Blend blend_src;
    Blend blend_dst;
    BlendOp blend_op;
    Blend blend_src_alpha;
    Blend blend_dst_alpha;
    BlendOp blend_op_alpha;
};

struct RasterizerStageInfo {
    FillMode fill_mode;
    CullMode cull_mode;
};

enum class CommandBufferType {
    Graphics,
    Copy,
    Compute
};

class CommandBuffer : public core::ref_counted_object {
public:

    virtual auto close() -> void = 0;

    virtual auto set_graphics_pipeline_options(
        core::ref_ptr<Shader> shader,
        RasterizerStageInfo const& rasterizer,
        BlendColorInfo const& blend_color,
        std::optional<DepthStencilStageInfo> const depth_stencil
    ) -> void = 0;

    virtual auto begin_render_pass(
        std::span<RenderPassColorInfo const> const colors,
        std::optional<RenderPassDepthStencilInfo> depth_stencil
    ) -> void = 0;

    virtual auto end_render_pass() -> void = 0;

    virtual auto bind_vertex_buffer(uint32_t const slot, core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void = 0;

    virtual auto bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size, IndexFormat const format) -> void = 0;

    virtual auto draw_indexed(uint32_t const index_count, uint32_t const instance_count, uint32_t instance_offset) -> void = 0;

    virtual auto copy_buffer(
        core::ref_ptr<Buffer> dst, 
        uint64_t const dst_offset, 
        core::ref_ptr<Buffer> src, 
        uint64_t const src_offset,
        size_t const size
    ) -> void = 0;
};

}

}

}