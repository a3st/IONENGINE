// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class FrameGraphRenderPass {
public:

    FrameGraphRenderPass(const std::string& name, std::function<void(RenderPassContext&)> exec_func)
        : m_name(name), m_exec_func(exec_func) {

    }

    const std::string& get_name() const { return m_name; }
    void execute(RenderPassContext& context) { m_exec_func(context); }

    void add_write(FrameGraphResource& resource) { m_writes.emplace_back(resource); }
    void add_read(FrameGraphResource& resource) { m_reads.emplace_back(resource); }

    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_writes() const { return m_writes; }
    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_reads() const { return m_reads; }

private:

    std::string m_name;

    std::function<void(RenderPassContext&)> m_exec_func;

    std::vector<std::reference_wrapper<FrameGraphResource>> m_writes;
    std::vector<std::reference_wrapper<FrameGraphResource>> m_reads;
};

}