// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class FrameGraphRenderPass {
friend class RenderPassBuilder;
public:

    FrameGraphRenderPass(const std::string& name, std::function<void(RenderPassContext&)> exec_func)
        : m_name(name), m_exec_func(exec_func) {

    }

    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_outputs() const { return m_outputs; }
    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_inputs() const { return m_inputs; }
    const std::string& get_name() const { return m_name; }
    void execute(RenderPassContext& context) { m_exec_func(context); }

protected:

    void add_output(FrameGraphResource& resource) { m_outputs.emplace_back(resource); }
    void add_input(FrameGraphResource& resource) { m_inputs.emplace_back(resource); }

private:

    std::string m_name;

    std::function<void(RenderPassContext&)> m_exec_func;

    std::vector<std::reference_wrapper<FrameGraphResource>> m_outputs;
    std::vector<std::reference_wrapper<FrameGraphResource>> m_inputs;
};

}