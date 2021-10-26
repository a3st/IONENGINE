// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"
#include "../lstd/object_pool_ptr.h"

#include "color.h"

#include "frame_buffer_cache.h"
#include "render_pass_cache.h"
#include "texture_view_cache.h"
#include "texture_cache.h"

namespace ionengine::rendersystem {

const size_t kFrameGraphTaskSize = 50;
const size_t kFrameGraphResourceSize = 50;

enum class FrameGraphTaskType {

    kRenderPass,
    kComputePass
};

enum class FrameGraphResourceType {

    kAttachment,
    kBuffer
};

enum class FrameGraphResourceOp {

    kLoad,
    kClear
};

class FrameGraphResourceId {

friend class FrameGraphBuilder;

public:

    FrameGraphResourceId() : id_(std::numeric_limits<uint32_t>::max()) { 

    }

    FrameGraphResourceId(const uint32_t id) : id_(id) {

    }

    inline bool operator==(const FrameGraphResourceId& rhs) const { return id_ == rhs.id_; }
    inline bool operator!=(const FrameGraphResourceId& rhs) const { return id_ != rhs.id_; }

private:

    uint32_t id_;
};

class FrameGraphResource {

public:

    FrameGraphResource() {

    }

    FrameGraphResource(const FrameGraphResourceType type, lgfx::Texture* texture, lgfx::TextureView* texture_view) :
        type_(type), texture_(texture), texture_view_(texture_view) {

    }

    inline lgfx::Texture* GetTexture() const { return texture_; }
    inline lgfx::TextureView* GetTextureView() const { return texture_view_; }
    inline FrameGraphResourceType GetType() const { return type_; }

private:

    lgfx::Texture* texture_;
    lgfx::TextureView* texture_view_;

    FrameGraphResourceType type_;
};

class FrameGraphTaskId {

friend class FrameGraphBuilder;

public:

    FrameGraphTaskId() : id_(std::numeric_limits<uint32_t>::max()) { 

    }

    FrameGraphTaskId(const uint32_t id) : id_(id) {

    }

    inline bool operator==(const FrameGraphTaskId& rhs) const { return id_ == rhs.id_; }
    inline bool operator!=(const FrameGraphTaskId& rhs) const { return id_ != rhs.id_; }

private:

    uint32_t id_;
};

class FrameGraphTask {

public:

    FrameGraphTask() {

    }

    FrameGraphTask(const FrameGraphTaskType type) : type_(type) {

    }

private:

    FrameGraphTaskType type_;

    std::span<FrameGraphResourceId> create_resources;
    std::span<FrameGraphResourceId> write_resources;
    std::span<FrameGraphResourceId> read_resources;

    std::span<lgfx::ClearValueColor> clear_colors;
    std::span<lgfx::RenderPassColorDesc> color_descs;
    std::span<lgfx::TextureView*> color_views;
};

struct FrameGraphResourceDesc {

    FrameGraphResourceType type;
    uint32_t width;
    uint32_t height;
    uint32_t mip_levels;
    uint32_t array_layers;
    lgfx::Format format;
    lgfx::TextureFlags flags;
    lgfx::Texture* texture;
    lgfx::TextureView* texture_view;
};

class FrameGraphBuilder {

public:

    FrameGraphBuilder(TextureCache* texture_cache, TextureViewCache* texture_view_cache, std::vector<FrameGraphResource>* resources, std::vector<FrameGraphTask>* tasks);

    inline FrameGraphTaskId GetResult() { return task_id_; }

    FrameGraphResourceId Create(const FrameGraphResourceDesc& desc);

    //void Read(const FrameGraphResourceId& resource_id);
    void Write(const FrameGraphResourceId& resource_id, const FrameGraphResourceOp op, const Color& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f });

private:

    //lgfx::RenderPassLoadOp FrameGraphResourceWriteOpTo(const FrameGraphResourceWriteOp op);

    FrameGraphTaskId task_id_;

    TextureCache* texture_cache_;
    TextureViewCache* texture_view_cache_; 
    std::vector<FrameGraphResource>* resources_;
    std::vector<FrameGraphTask>* tasks_;
};

class FrameGraphContext {

public:

    FrameGraphContext();

};

class FrameGraph {

public:

    FrameGraph(lgfx::Device* device);

    FrameGraphTaskId AddTask(const FrameGraphTaskType type, 
        const std::function<void(FrameGraphBuilder*)>& builder_func, 
        const std::function<void(FrameGraphContext*)>& exec_func);

    void Execute();
    void Flush();

private:

    lgfx::Device* device_;

    std::unique_ptr<FrameBufferCache> frame_buffer_cache_;
    std::unique_ptr<RenderPassCache> render_pass_cache_;
    std::unique_ptr<TextureViewCache> texture_view_cache_;
    std::unique_ptr<TextureCache> texture_cache_;

    std::unique_ptr<lgfx::CommandBuffer> command_buffer_;

    std::vector<FrameGraphTask> tasks_;
    std::vector<FrameGraphResource> resources_;

    lstd::object_pool<lgfx::RenderPassColorDesc> color_descs;
};

}