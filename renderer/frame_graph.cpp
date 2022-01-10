// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine::renderer;


FrameGraphBuilder::FrameGraphBuilder() {

}

FGResourceHandle FrameGraphBuilder::create(FGResourceType const res_type, GPUResourceHandle const& handle) {

    return FGResourceHandle { };
}

void FrameGraph::wait_until(FGTaskHandle const& handle) {

}

void FrameGraph::execute(RenderQueue& queue) {

    //for(uint32_t i = 0; i < tasks_.size(); ++i) {

        //std::cout << std::format("task {}, creates {}, writes {}", i, tasks_[i].creates.size(), tasks_[i].writes.size()) << std::endl;
    //}

    //tasks_.clear();
    //resources_.clear();
}