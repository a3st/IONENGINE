// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>

namespace ionengine::renderer {

class ShaderUniformPool {
public:

    ShaderUniformPool(backend::Handle<backend::DescriptorLayout> const& descriptor_layout);

private:

    //backend::Handle<backend::DescriptorSet>
};

}