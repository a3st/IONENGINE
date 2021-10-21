// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

#include "frame_buffer_cache.h"
#include "render_pass_cache.h"

namespace ionengine::rendersystem {

enum class FrameGraphTaskType {
    RenderPass,
    ComputePass
};

class FrameGraphTaskBuilder {

public:

    FrameGraphTaskBuilder();

private:

};

class FrameGraphTaskContext {

public:

    FrameGraphTaskContext();

private:

};

class FrameGraph {

public:

    FrameGraph();
    FrameGraph(lgfx::Device* device);
    FrameGraph(const FrameGraph&) = delete;
    FrameGraph(FrameGraph&& rhs) noexcept;

    FrameGraph& operator=(const FrameGraph&) = delete;
    FrameGraph& operator=(FrameGraph&& rhs) noexcept;

    template<typename T>
    T AddTask(const FrameGraphTaskType type, const std::function<void(FrameGraphTaskBuilder*, T&)>& builder_func, const std::function<void(FrameGraphTaskContext*)>& exec_func);

    void Execute();

private:

    FrameBufferCache frame_buffer_cache;
    RenderPassCache render_pass_cache;

};

}