// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

#include "../lstd/object_pool.h"

#include "color.h"

#include "cache/frame_buffer.h"
#include "cache/render_pass.h"
#include "cache/texture_view.h"
#include "cache/texture.h"

namespace ionengine::rendersystem {

const size_t kFrameGraphResourcePoolSize = 128;
const size_t kFrameGraphTaskPoolSize = 64;

enum class FrameGraphResourceType {

    kAttachment,
    kBuffer
};

enum class FrameGraphResourceOp {

    kLoad,
    kClear
};

class FrameGraphResource {

friend class FrameGraphBuilder;

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

enum class FrameGraphTaskType {

    kUnknown,
    kRenderPass,
    kComputePass
};

class FrameGraphTask {

friend class FrameGraphTaskCache;

public:

    FrameGraphTask() {

    }

    FrameGraphTask(const FrameGraphTaskType type) : type_(type) {

    }

private:

    FrameGraphTaskType type_;

    std::vector<FrameGraphResource*> create_resources_;
    std::vector<FrameGraphResource*> write_resources_;
    std::vector<FrameGraphResource*> read_resources_;

    std::vector<lgfx::ClearValueColor> clear_colors_;
    std::vector<lgfx::RenderPassColorDesc> color_descs_;
    std::vector<lgfx::TextureView*> color_views_;
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

    FrameGraphBuilder(
        TextureCache* texture_cache, 
        TextureViewCache* texture_view_cache, 
        lstd::object_pool<FrameGraphResource>* resource_pool, 
        lstd::object_pool<FrameGraphTask>* task_pool,
        std::vector<lstd::unique_object_ptr<FrameGraphResource>>& resources,
        std::vector<lstd::unique_object_ptr<FrameGraphTask>>& tasks);

    FrameGraphResource* Create(const FrameGraphResourceDesc& desc);
    void Read(FrameGraphResource* resource);
    void Write(FrameGraphResource* resource, const FrameGraphResourceOp op, const Color& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f });

    inline FrameGraphTask* operator()() {

        return task_;
    }

private:

    TextureCache* texture_cache_;
    TextureViewCache* texture_view_cache_;

    lstd::object_pool<FrameGraphResource>* resource_pool_;
    lstd::object_pool<FrameGraphTask>* task_pool_;

    std::vector<lstd::unique_object_ptr<FrameGraphResource>>& resources_;
    std::vector<lstd::unique_object_ptr<FrameGraphTask>>& tasks_;

    FrameGraphTask* task_;
};

class FrameGraphContext {

public:

    FrameGraphContext() {
        
    }

};

class FrameGraph {

public:

    FrameGraph(lgfx::Device* device);
    FrameGraph(const FrameGraph&) = delete;
    FrameGraph(FrameGraph&&) = delete;

    FrameGraph& operator=(const FrameGraph&) = delete;
    FrameGraph& operator=(FrameGraph&&) = delete;

    FrameGraphTask* AddTask(const FrameGraphTaskType type, const std::function<void(FrameGraphBuilder*)>& builder_func, const std::function<void(FrameGraphContext*)>& exec_func);

    void Execute();

    void Reset();

    void Flush();

private:

    lgfx::Device* device_;

    std::unique_ptr<lgfx::CommandBuffer> command_buffer_;

    FrameBufferCache frame_buffer_cache_;
    RenderPassCache render_pass_cache_;
    TextureViewCache texture_view_cache_;
    TextureCache texture_cache_;

    lstd::object_pool<FrameGraphResource> resource_pool;
    lstd::object_pool<FrameGraphTask> task_pool;

    std::vector<lstd::unique_object_ptr<FrameGraphResource>> resources_;
    std::vector<lstd::unique_object_ptr<FrameGraphTask>> tasks_;
};

}