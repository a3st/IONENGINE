// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class FrameGraphRenderPass {
public:

    FrameGraphRenderPass() {

    }

private:

    std::function<void(RenderPassContext&)> m_exec_func;
};

}