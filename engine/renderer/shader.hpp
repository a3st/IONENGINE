#pragma once

#include "engine/asset.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class LinkedDevice;

    class Shader : public Asset
    {
      public:
        Shader(LinkedDevice& device);

      private:
        LinkedDevice* device;
        
    };
} // namespace ionengine