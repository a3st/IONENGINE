// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine
{
    class IEngineModule : public core::ref_counted_object
    {
      public:
        virtual ~IEngineModule() = default;
    };
}