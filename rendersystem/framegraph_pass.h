// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rendersystem {

class FrameGraphPassContext {
public:

    FrameGraphPassContext(gfx::CommandList* command_list) : m_command_list(command_list) {


    }

    gfx::CommandList* get_command_list() { return m_command_list; }

private:

    gfx::CommandList* m_command_list;

};

class FrameGraphPass {
public:

    FrameGraphPass(
        const std::string& name,
        const std::function<void(FrameGraphPassContext*)>& exec_func
    ) : 
        m_name(name),
        m_exec_func(exec_func) {

    }

    void execute(FrameGraphPassContext* context) const { m_exec_func(context); }

    void read_resource(const ReadFrameGraphResource& read_resource) { m_read_resources.emplace_back(read_resource); }
 
    void write_resource(const WriteFrameGraphResource& write_resource) { m_write_resources.emplace_back(write_resource); }

    void create_resource(FrameGraphResource* resource) { m_create_resources.emplace_back(resource); }

    const std::vector<WriteFrameGraphResource>& get_write_resources() const { return m_write_resources; }

    const std::vector<ReadFrameGraphResource>& get_read_resources() const { return m_read_resources; }
    
    const std::vector<FrameGraphResource*>& get_create_resources() const { return m_create_resources; }

private:

    const std::string m_name;

    std::function<void(FrameGraphPassContext*)> m_exec_func;

    std::vector<WriteFrameGraphResource> m_write_resources;

    std::vector<ReadFrameGraphResource> m_read_resources;

    std::vector<FrameGraphResource*> m_create_resources;
};

}