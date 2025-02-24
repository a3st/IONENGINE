// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class Graphics : public core::ref_counted_object
    {
      public:
        Graphics(core::ref_ptr<rhi::RHI> RHI);

      private:
        core::ref_ptr<rhi::RHI> RHI;
    };
} // namespace ionengine