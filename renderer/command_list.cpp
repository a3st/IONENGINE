// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/command_list.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<CommandList, lib::Result<CommandListError>> CommandList::create(backend::Device& device, backend::QueueFlags const flags, bool const bundle) {

    auto command_list = CommandList {};
    command_list.command_list = device.create_command_list(flags, bundle);
    command_list.flags = flags;
    command_list.is_bundle = bundle;

    return lib::make_expected<CommandList, lib::Result<CommandListError>>(std::move(command_list));
}
