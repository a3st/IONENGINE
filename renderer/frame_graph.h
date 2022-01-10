// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/color.h>

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

enum class FGResourceFlags : uint16_t {
    Presentable = 1 << 0,
    DepthStencil = 1 << 1
};

enum class FrameGraphResourceOp {
    Load,
    Clear
};

struct FrameGraphResourceClearDesc {
    Color color;
    uint8_t stencil;
    float depth;
};

/*struct FrameGraphResource {
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
};*/

class FrameGraphBuilder {
public:

    FrameGraphBuilder();

    FGResourceHandle create(FGResourceType const res_type, GPUResourceHandle const& handle);

private:

};

class FrameGraphContext {
public:


private:

};

class FrameGraph {
public:

    FrameGraph() = default;

    template<class Data>
    FGTaskHandle add_task(
        FGTaskType const task_type,
        std::function<void(FrameGraphBuilder&, Data&)> const build_func, 
        std::function<void(FrameGraphContext&, Data const&)> context_func
    ) {

        return FGTaskHandle {};
    }

    void wait_until(FGTaskHandle const& handle);

    template<class Data>
    Data& get_data(FGTaskHandle const& handle) {

        Data data{};
        return data;
    }

    void execute(RenderQueue& queue);

private:

    //std::vector<FrameGraphResource> resources_;
    //std::vector<FrameGraphTask> tasks_;

};

}