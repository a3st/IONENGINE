// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.h>

namespace ionengine::renderer {

class CommandList;

///
/// Render pass load operations
///
enum class RenderPassLoadOp { Load, Clear, DontCare };

///
/// Render pass store operations
///
enum class RenderPassStoreOp { Store, DontCare };

///
/// Render pass color description
///
struct RenderPassColorDesc {
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
};

///
/// Render pass depth stencil description
///
struct RenderPassDepthStencilDesc {
    RenderPassLoadOp depth_load_op;
    RenderPassStoreOp depth_store_op;
    RenderPassLoadOp stencil_load_op;
    RenderPassStoreOp stencil_store_op;
};

class RenderPass {
 public:
    virtual void draw(CommandList& command_list, uint32_t const vertex_count,
                      uint32_t const instance_count,
                      uint32_t const vertex_offset) = 0;

    virtual void draw_indexed(CommandList& command_list,
                              uint32_t const index_count,
                              uint32_t const instance_count,
                              uint32_t const instance_offset) = 0;

    virtual void bind_vertex_buffer(CommandList& command_list,
                                    uint32_t const slot, Buffer& buffer) = 0;

    virtual void bind_index_buffer(CommandList& command_list,
                                   Buffer& buffer) = 0;

    
};

}  // namespace ionengine::renderer