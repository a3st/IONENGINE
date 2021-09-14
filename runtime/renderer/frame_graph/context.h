// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassContext {
public:

    RenderPassContext(CommandList& command_list) : m_command_list(command_list) {

    }

    CommandList& get_command_list() { return m_command_list; }

private:

    std::reference_wrapper<CommandList> m_command_list;
};

class ComputePassContext {
public:

    ComputePassContext(CommandList& command_list) : m_command_list(command_list) {

    }

    CommandList& get_command_list() { return m_command_list; }

private:

    std::reference_wrapper<CommandList> m_command_list;
};

}