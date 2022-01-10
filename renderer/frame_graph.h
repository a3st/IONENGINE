// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/color.h>

namespace ionengine::renderer {

enum class FrameGraphTaskType {
    RenderPass,
    ComputePass
};

enum class FrameGraphResourceType {
    Attachment,
    Buffer
};

enum class FrameGraphResourceFlags {
    Presentable = 1 << 0,
    DepthStencil = 1 << 1
};

enum class FrameGraphResourceOp {
    Load,
    Clear
};

HELPER_DEFINE_HANDLE(FrameGraphResourceId)

struct FrameGraphResourceClearDesc {
    Color color;
    uint8_t stencil;
    float depth;
};

struct FrameGraphResource {
    FrameGraphResourceType resource_type;
    uint32_t width;
    uint32_t height;
    FrameGraphResourceFlags flags;
};

struct FrameGraphResourceWrite {
    FrameGraphResourceId id;
    FrameGraphResourceOp op;
    FrameGraphResourceClearDesc clear_desc;
};

struct FrameGraphTask {
    FrameGraphTaskType task_type;
    std::vector<FrameGraphResourceId> creates;
    std::vector<FrameGraphResourceWrite> writes;
};

class FrameGraphBuilder {
public:

    FrameGraphBuilder(std::vector<FrameGraphResource>& resources, FrameGraphTask& task);

    FrameGraphResourceId create(FrameGraphResource const& desc);
    void write(FrameGraphResourceId const& id, FrameGraphResourceOp const op, FrameGraphResourceClearDesc const& clear_desc);

private:

    std::vector<FrameGraphResource>* resources_;
    FrameGraphTask* task_;

};

class FrameGraphContext {
public:


private:

};

class FrameGraph {
public:

    FrameGraph(Backend& backend);

    template<class T>
    void add_task(
        FrameGraphTaskType const task_type,
        std::function<void(FrameGraphBuilder&, T&)> const build_func, 
        std::function<void(FrameGraphContext&, T const&)> context_func
    ) {

        tasks_.emplace_back(FrameGraphTask { task_type });
        
        T data{};
        FrameGraphBuilder builder(resources_, tasks_.back());
        build_func(builder, data);
    }

    void execute();

private:

    std::vector<FrameGraphResource> resources_;
    std::vector<FrameGraphTask> tasks_;

};

}