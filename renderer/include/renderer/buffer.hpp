// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.hpp>

namespace ionengine::renderer {

enum class BufferUsageFlags : uint32_t {
    Constant = 1 << 0,
    ShaderResource = 1 << 1,
    UnorderedAccess = 1 << 2
};

DECLARE_ENUM_CLASS_BIT_FLAG(BufferUsageFlags)

enum class MemoryUsage { Static, Dynamic };

class Buffer {
 public:
    ///
    /// Creates buffer
    /// @param size Size buffer in bytes
    /// @param stride Stride element in bytes for structure buffers
    /// @param flags Usage flags buffer
    /// @param memory Memory usage
    /// @return Returns unique pointer to buffer
    ///
    static core::Expected<std::unique_ptr<Buffer>, std::string> create(
        Device& device, size_t const size, uint32_t const stride,
        BufferUsageFlags const flags, MemoryUsage const memory) noexcept;

    
};

}  // namespace ionengine::renderer