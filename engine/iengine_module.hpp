// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine
{
    enum class EngineModulePriority : uint16_t
    {
        System = 0,
        Core = 100,
        SubSystem = 200
    };

    class IEngineModule : public core::ref_counted_object
    {
      public:
        virtual ~IEngineModule() = default;

        virtual auto initialize() -> void = 0;

        virtual auto shutdown() -> void = 0;

        virtual auto getPriority() const -> uint16_t = 0;
    };
} // namespace ionengine