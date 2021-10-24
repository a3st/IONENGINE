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
const size_t kFrameGraphResourceSize = 50;

enum class FrameGraphTaskType {
    kRenderPass,
    kComputePass
};

enum class FrameGraphResourceType {
    kAttachment,
    kBuffer
};

enum class FrameGraphResourceWriteOp {
    kLoad,
    kClear
};

struct FrameGraphResourceDesc {
    FrameGraphResourceType type;
    lgfx::Texture* texture;
    lgfx::TextureView* texture_view;
};

class FrameGraphResourceId {

friend class FrameGraphBuilder;

public:

    FrameGraphResourceId() : id_(std::numeric_limits<uint32_t>::max()) { 

    }

    FrameGraphResourceId(const uint32_t id) : id_(id) {

    }

private:

    uint32_t id_;
};

struct FrameGraphTaskDesc {
    FrameGraphTaskType type;
    std::span<FrameGraphResourceId> write_resources;
    std::span<lgfx::ClearValueColor> clear_colors;
    std::span<lgfx::RenderPassColorDesc> color_descs;
    std::span<lgfx::TextureView*> color_views;
};

struct FrameGraphResourceCreateInfo {
    FrameGraphResourceType type;
    uint32_t width;
    uint32_t height;
    lgfx::Texture* texture;
    lgfx::TextureView* texture_view;
};

class FrameGraphBuilder {

public:

    FrameGraphBuilder(TextureCache* texture_cache, TextureViewCache* texture_view_cache, const uint32_t task_index, std::vector<FrameGraphTaskDesc>& tasks, std::vector<FrameGraphResourceDesc>& resources);

    FrameGraphResourceId Create(const FrameGraphResourceCreateInfo& create_info);
    void Read(const FrameGraphResourceId& resource_id);
    void Write(const FrameGraphResourceId& resource_id, const FrameGraphResourceWriteOp write_op, const Color& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f });

private:

    lgfx::RenderPassLoadOp FrameGraphResourceWriteOpTo(const FrameGraphResourceWriteOp op);

    uint32_t task_index_; 
    std::vector<FrameGraphTaskDesc>& tasks_;
    std::vector<FrameGraphResourceDesc>& resources_;

    TextureCache* texture_cache_; 
    TextureViewCache* texture_view_cache_; 
};

class FrameGraphContext {

public:

    FrameGraphContext();

};

class FrameGraph {

public:

    FrameGraph(lgfx::Device* device);

    int32_t AddTask(const FrameGraphTaskType type, 
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

    std::vector<FrameGraphTaskDesc> tasks_;
    uint32_t task_index_;
    std::vector<FrameGraphResourceDesc> resources_;
};

}