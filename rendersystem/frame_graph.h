// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

#include "color.h"

#include "frame_buffer_cache.h"
#include "render_pass_cache.h"
#include "texture_view_cache.h"
#include "texture_cache.h"

namespace ionengine::rendersystem {

const size_t kFrameGraphTaskSize = 50;

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

friend class FrameGraphBuilder;

public:

    FrameGraphResource();
    FrameGraphResource(lgfx::Texture* texture, lgfx::TextureView* texture_view);

    inline FrameGraphResourceType GetType() const { return type_; }

private:

    FrameGraphResourceType type_;

    struct Attachment {
        lgfx::Texture* texture;
        lgfx::TextureView* texture_view;
    };

    struct Buffer {
        lgfx::Texture* buffer;
    };

    union {
        Attachment attachment_;
        Buffer buffer_;
    };
};

class FrameGraphTask {

friend class FrameGraphBuilder;
friend class FrameGraph;

public:

    FrameGraphTask();

    inline FrameGraphResource* GetResource(const uint32_t index) { return &resources_[index]; }

    void Clear();

private:

    std::vector<FrameGraphResource> resources_;

    std::vector<lgfx::Texture*> write_textures_;

    lgfx::ClearValueDesc clear_desc_;
    lgfx::RenderPassDesc render_pass_desc_;
    lgfx::FrameBufferDesc frame_buffer_desc_;
};

struct FrameGraphExternalResourceInfo {
    struct Attachment {
        lgfx::Texture* texture;
        lgfx::TextureView* texture_view;
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

class FrameGraphBuilder {

public:

    FrameGraphBuilder(FrameGraphTask* task);

    FrameGraphResource* Create(const FrameGraphResourceType type, const FrameGraphExternalResourceInfo& info);
    FrameGraphResource* Create(const FrameGraphResourceType type, const FrameGraphResourceInfo& info);
    void Read(FrameGraphResource* resource);
    void Write(FrameGraphResource* resource, const FrameGraphResourceWriteOp write_op, const Color& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f });

private:

    FrameGraphTask* task_;

};

class FrameGraphContext {

public:

    FrameGraphContext();

};

class FrameGraph {

public:

    FrameGraph();
    FrameGraph(lgfx::Device* device);
    FrameGraph(const FrameGraph&) = delete;
    FrameGraph(FrameGraph&& rhs) noexcept;

    FrameGraph& operator=(const FrameGraph&) = delete;
    FrameGraph& operator=(FrameGraph&& rhs) noexcept;

    FrameGraphTask* AddTask(const FrameGraphTaskType type, 
        const std::function<void(FrameGraphBuilder*)>& builder_func, 
        const std::function<void(FrameGraphContext*)>& exec_func);

    void Execute();
    void Flush();

private:

    lgfx::Device* device_;

    FrameBufferCache frame_buffer_cache_;
    RenderPassCache render_pass_cache_;
    TextureViewCache texture_view_cache_;
    TextureCache texture_cache_;

    lgfx::CommandBuffer command_buffer_;

    std::vector<FrameGraphTask> tasks_;
    uint32_t task_index_;
};

}