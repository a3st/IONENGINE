// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

/**
    @brief Graphics API CommandList class

    Command List class for Graphics API. It's needed for create command lists and dispath it to device
*/
class CommandBuffer {
public:

    virtual ~CommandBuffer() = default;
    
    /**
        @brief Bind pipeline to command list
        @param pipeline reference to Pipeline object
    */
    virtual void bind_pipeline(Pipeline& pipeline) = 0;

    /**
        @brief Close command list
    */
    virtual void close() = 0;

    /**
        @brief Reset command list 
    */
    virtual void reset() = 0;

    /**
        @brief Set the viewport to command list
        @param x viewport x position
        @param y viewport y position
        @param width viewport width
        @param height viewport height
    */
    virtual void set_viewport(const int32 x, const int32 y, const uint32 width, const uint32 height) = 0;
    
    /**
        @brief Set the scissor rect to command list
        @param left scissor left position
        @param top scissor top position
        @param right scissor right position
        @param bottom scissor bottom position
    */
    virtual void set_scissor_rect(int32 left, int32 top, int32 right, int32 bottom) = 0;

    /**
        @brief Set resource barriers to command list
        @param barriers vector of ResourceBarrierDescs
    */
    virtual void resource_barriers(const std::vector<ResourceBarrierDesc>& barriers) = 0;
    
    /**
        @brief Begin render pass in command list
        @param render_pass reference to RenderPass object 
        @param frame_buffer reference to FrameBuffer object
        @param clear_desc ClearValueDesc
    */
    virtual void begin_render_pass(RenderPass& render_pass, FrameBuffer& frame_buffer, const ClearValueDesc& clear_desc) = 0;
    
    /**
        @brief End render pass in command list 
    */
    virtual void end_render_pass() = 0;
    
    /**
        @brief Draw indexed triangles in command list
        @param index_count Count of indices
        @param instance_count Count of instances
        @param first_index First index
        @param vertex_offset Vertex offset
        @param first_instance First instance
    */
    virtual void draw_indexed(const uint32 index_count, const uint32 instance_count, const uint32 first_index, const int32 vertex_offset, const uint32 first_instance) = 0;
    
    /**
        @brief Set the index buffer in command list
        @param resource Resource to bind to index buffer
        @param format Index buffer format
    */
    virtual void set_index_buffer(Resource& resource, const Format format) = 0;

    /**
        @brief Set the vertex buffer in command list
        @param slot Vertex buffer slot
        @param resource Resource to bind to vertex buffer 
        @param stride Vertex buffer stride
    */
    virtual void set_vertex_buffer(const uint32 slot, Resource& resource, const uint32 stride) = 0;
};

}