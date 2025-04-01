// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class GUI
    {
      public:
        GUI(core::ref_ptr<rhi::RHI> RHI);

      private:
        inline static GUI* instance;

      public:
        
    };
} // namespace ionengine