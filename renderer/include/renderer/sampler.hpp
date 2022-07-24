// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.hpp>

namespace ionengine::renderer {

///
/// Filters
///
enum class Filter { Anisotropic, MinMagMipLinear, ComparisonMinMagMipLinear };

///
/// Address modes
///
enum class AddressMode { Wrap, Clamp, Mirror };

class Sampler {
 public:

    ///
    /// Create device
    ///
    static core::Expected<std::unique_ptr<Sampler>, std::string> create(
        
    ) noexcept;
};

}  // namespace ionengine::renderer