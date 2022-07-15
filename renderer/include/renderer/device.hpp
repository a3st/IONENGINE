// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.h>
#include <platform/window.hpp>

namespace ionengine::renderer {

uint32_t constexpr BACK_BUFFER_COUNT = 2;

class Device {
 public:

    ///
    /// Create device
    ///
    static core::Expected<std::unique_ptr<Device>, std::string> create(platform::Window& window, uint16_t const sample_count) noexcept;
};

}  // namespace ionengine::renderer