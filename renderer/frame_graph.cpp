// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/framegraph.h>

using namespace ionengine::renderer;


FrameGraphBuilder::FrameGraphBuilder(std::vector<FrameGraphResource>& resources, FrameGraphTask& task) : resources_(&resources), task_(&task) {

}

FrameGraphResourceId FrameGraphBuilder::create(FrameGraphResource const& desc) {

    resources_->emplace_back(desc);
    task_->creates.emplace_back(resources_->size() - 1);
    return FrameGraphResourceId(resources_->size() - 1);
}

void FrameGraphBuilder::write(FrameGraphResourceId const& id, FrameGraphResourceOp const op, FrameGraphResourceClearDesc const& clear_desc) {

    task_->writes.emplace_back(FrameGraphResourceWrite {
            id,
            op,
            clear_desc
        }
    );
}


FrameGraph::FrameGraph(Backend& backend) {
    
}

void FrameGraph::execute() {

    for(uint32_t i = 0; i < tasks_.size(); ++i) {

        //std::cout << std::format("task {}, creates {}, writes {}", i, tasks_[i].creates.size(), tasks_[i].writes.size()) << std::endl;
    }

    tasks_.clear();
    resources_.clear();
}