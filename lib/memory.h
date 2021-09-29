// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::memory_literals {

constexpr usize operator ""_kb(const usize size) {
    return size * 1024;
}

constexpr usize operator ""_mb(const usize size) {
    return size * 1024 * 1024;
}

constexpr usize operator ""_gb(const usize size) {
    return size * 1024 * 1024 * 1024;
}

}