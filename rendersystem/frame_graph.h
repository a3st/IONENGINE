// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

#include "color.h"

#include "frame_buffer_cache.h"
#include "render_pass_cache.h"

namespace ionengine::rendersystem {

enum class FrameGraphTaskType {
    RenderPass,
    ComputePass
};

enum class FrameGraphResourceType {
    Attachment,
    Buffer
};

enum class FrameGraphResourceWriteOp {
    Load,
    Clear
};

class FrameGraphResource {

public:

    FrameGraphResource();
    FrameGraphResource(lgfx::Texture* texture, lgfx::TextureView* texture_view);

    inline FrameGraphResourceType GetType() const { return type_; }

private:

    FrameGraphResourceType type_;

    struct Attachment {
        lgfx::Texture* texture;
        lgfx::TextureView* view;
    };

    struct Buffer {
    };

    union {
        Attachment attachment_;
        Buffer buffer_;
    };
};

struct FrameGraphExternalResourceInfo {
    struct Attachment {
        lgfx::Texture* texture;
        lgfx::TextureView* view;
    };

    union {
        Attachment attachment;
    };
};

struct FrameGraphResourceInfo {
    struct Attachment {
        uint32_t width;
        uint32_t height;
    };

    union {
        Attachment attachment;
    };
};

class FrameGraphTaskBuilder {

public:

    FrameGraphTaskBuilder();

    FrameGraphResource Create(const FrameGraphResourceType type, const FrameGraphExternalResourceInfo& info);
    FrameGraphResource Create(const FrameGraphResourceType type, const FrameGraphResourceInfo& info);
    void Read(FrameGraphResource* resource);
    void Write(FrameGraphResource* resource, const FrameGraphResourceWriteOp write_op, const Color& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f });

};

class FrameGraphTaskContext {

public:

    FrameGraphTaskContext();

};

class FrameGraph {

public:

    FrameGraph();
    FrameGraph(lgfx::Device* device);
    FrameGraph(const FrameGraph&) = delete;
    FrameGraph(FrameGraph&& rhs) noexcept;

    FrameGraph& operator=(const FrameGraph&) = delete;
    FrameGraph& operator=(FrameGraph&& rhs) noexcept;

    template<typename T>
    T AddTask(const FrameGraphTaskType type, const std::function<void(FrameGraphTaskBuilder*, T&)>& builder_func, const std::function<void(FrameGraphTaskContext*, T&)>& exec_func) {
        
        T data{};

        FrameGraphTaskBuilder builder;
        builder_func(&builder, data);

        // Reset command
        //command_buffer_.Reset();

        // Barriers



        FrameGraphTaskContext context;
        exec_func(&context, data);
        
        // Barriers end
        return data;
    }

    void Execute();

private:

    FrameBufferCache frame_buffer_cache_;
    RenderPassCache render_pass_cache_;

    lgfx::CommandBuffer command_buffer_;
};

}