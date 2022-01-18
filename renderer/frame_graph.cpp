// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frame_graph.h>

using namespace ionengine::renderer;


FrameGraphBuilder::FrameGraphBuilder(FrameGraph& frame_graph) : _frame_graph(&frame_graph) {

}

FGResourceHandle FrameGraphBuilder::create(FGResourceType const res_type, GPUResourceHandle const& handle) {

    size_t offset = _frame_graph->_resources.push(
        FGResource {
            res_type,
            handle
        }
    );
    std::cout << std::format("FG: Created resource (ID: {})", offset) << std::endl;
    return FGResourceHandle { static_cast<uint32_t>(offset) };
}

void FrameGraph::wait_until(FGTaskHandle const& handle) {

}

void FrameGraph::execute() {

    //for(uint32_t i = 0; i < tasks_.size(); ++i) {

        //std::cout << std::format("task {}, creates {}, writes {}", i, tasks_[i].creates.size(), tasks_[i].writes.size()) << std::endl;
    //}

    //tasks_.clear();
    //resources_.clear();

    _resources.clear();
}