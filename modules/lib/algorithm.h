// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/expected.h>

namespace ionengine::lib {

std::span<uint8_t const> read_bytes(std::span<uint8_t const> const source,
                                    uint64_t& offset, size_t const size);

std::string_view get_line(std::string_view const buffer, size_t& offset,
                          char const delimeter);

enum class IOError { OpenError };

Expected<std::vector<uint8_t>, Result<IOError>> load_file(
    std::filesystem::path const& file_path);

}  // namespace ionengine::lib
