// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class CommandQueue {
public:

    virtual ~CommandQueue() = default;

    virtual void wait(Fence& fence, const uint64 value) = 0;
    virtual void signal(Fence& fence, const uint64 value) = 0;
    virtual void execute_command_lists(const std::vector<std::reference_wrapper<CommandList>>& command_lists) = 0;
};

}