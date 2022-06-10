// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class CommandListError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

struct CommandList {
    backend::Handle<backend::CommandList> command_list;
    backend::QueueFlags flags;
    bool is_bundle;

    static lib::Expected<CommandList, lib::Result<CommandListError>> create(backend::Device& device, backend::QueueFlags const flags, bool const bundle = false);
};

template<>
struct GPUResourceDeleter<CommandList> {
    void operator()(backend::Device& device, CommandList const& command_list) const {
        device.delete_command_list(command_list.command_list);
    }
};

}
