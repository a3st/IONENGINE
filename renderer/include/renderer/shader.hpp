// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.hpp>

namespace ionengine::renderer {

///
/// Shader usage flags
///
enum class ShaderUsageFlags : uint32_t {
    Vertex = 1 << 0,
    Geometry = 1 << 1,
    Domain = 1 << 2,
    Hull = 1 << 3,
    Pixel = 1 << 4,
    Compute = 1 << 5,
    All = Vertex | Geometry | Domain | Hull | Pixel | Compute
};

DECLARE_ENUM_CLASS_BIT_FLAG(ShaderUsageFlags)

class Shader {
 public:

    ///
    /// Create device
    ///
    static core::Expected<std::unique_ptr<Shader>, std::string> create(
        
    ) noexcept;
};

}  // namespace ionengine::renderer