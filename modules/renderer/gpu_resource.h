// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>

namespace ionengine::renderer {

template<class Type>
struct GPUResourceDeleter {
    void operator()(backend::Device& device, Type const&) const;
};

}