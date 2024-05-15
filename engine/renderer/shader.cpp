
#include "shader.hpp"
#include "linked_device.hpp"
#include "precompiled.h"

namespace ionengine
{
    Shader::Shader(LinkedDevice& device) : device(&device)
    {
    }
} // namespace ionengine