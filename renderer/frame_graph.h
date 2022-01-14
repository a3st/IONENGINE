// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/color.h>
#include <renderer/handle_pool.h>

namespace ionengine::renderer {

class FGResourceHandle {
public:

    FGResourceHandle() = default;
    FGResourceHandle(uint32_t const id) : _id(id) { }

private:
    
    uint32_t _id;
};

class FGTaskHandle {
public:

    FGTaskHandle() = default;
    FGTaskHandle(uint32_t const id) : _id(id) { }

private:
    
    uint32_t _id;
};

enum class FGTaskType {
    RenderPass,
    ComputePass,
    AsyncComputePass
};

enum class FGResourceType {
    Attachment,
    SwapchainAttachment,
    DepthStencilAttachment,
    Buffer
};

enum class FGResourceOp {
    Load,
    Clear
};

struct FrameGraphResourceClearDesc {
    Color color;
    uint8_t stencil;
    float depth;
};

struct FGResource {
    FGResourceType res_type;
    GPUResourceHandle handle;
};

struct FGTask {
    
};

class FrameGraphContext;
class FrameGraphBuilder;

class FrameGraph {
public:

    FrameGraph() = default;

    template<class Data>
    FGTaskHandle add_task(
        FGTaskType const task_type,
        std::function<void(FrameGraphBuilder&, Data&)> const build_func, 
        std::function<void(FrameGraphContext&, Data const&)> context_func
    );

    void wait_until(FGTaskHandle const& handle);

    template<class Data>
    Data& get_data(FGTaskHandle const& handle) {

        Data data{};
        return data;
    }

    void execute(RenderQueue& queue);

private:

    friend class FrameGraphBuilder;

    HandlePool<FGResource> _resources;
    HandlePool<FGTask> _tasks;
};

class FrameGraphContext {
public:


private:

};

class FrameGraphBuilder {
public:

    FrameGraphBuilder(FrameGraph& frame_graph);

    FGResourceHandle create(FGResourceType const res_type, GPUResourceHandle const& handle);

private:

    FrameGraph* _frame_graph;

};

template<class Data>
inline FGTaskHandle FrameGraph::add_task(
    FGTaskType const task_type,
    std::function<void(FrameGraphBuilder&, Data&)> const build_func, 
    std::function<void(FrameGraphContext&, Data const&)> context_func
) {

    Data data{};

    FrameGraphBuilder builder(*this);
    build_func(builder, data);

    return FGTaskHandle {};
}

}