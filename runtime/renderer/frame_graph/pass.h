// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

struct FrameGraphResourceDesc;

struct FrameGraphRenderPassDesc {
    std::string m_name;
    bool m_async;

    std::function<void(RenderPassContext&)> m_exec_func;

    std::vector<std::reference_wrapper<FrameGraphResourceDesc>> m_input_resources;
    std::vector<std::reference_wrapper<FrameGraphResourceDesc>> m_output_resources; 
};

}