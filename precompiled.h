// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <random>
#include <limits>
#include <functional>
#include <exception>
#include <cassert>
#include <vector>
#include <numeric>
#include <memory>
#include <array>
#include <queue>
#include <map>
#include <string>
#include <variant>
#include <span>
#include <string_view>
#include <ranges>
#include <tuple>
#include <unordered_set>
#include <semaphore>
#include <regex>
#include <spanstream>
#include <numbers>
#include <mutex>
#include <shared_mutex>

#ifndef DECLARE_ENUM_CLASS_BIT_FLAG
#define DECLARE_ENUM_CLASS_BIT_FLAG(EnumClass) \
using EnumClass##Flags = uint32_t; \
inline auto operator|(EnumClass const lhs, EnumClass const rhs) -> EnumClass##Flags { \
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs); \
} \
inline auto operator|(EnumClass##Flags const lhs, EnumClass const rhs) -> EnumClass##Flags { \
	return lhs | static_cast<uint32_t>(rhs); \
} \
inline auto operator&(EnumClass const lhs, EnumClass const rhs) -> bool { \
	return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs); \
} \
inline auto operator&(EnumClass##Flags const lhs, EnumClass const rhs) -> bool { \
	return lhs & static_cast<uint32_t>(rhs); \
}
#endif // DECLARE_ENUM_CLASS_BIT_FLAG

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template <class... Ts>
inline auto make_visitor(Ts... ts) {

    return overloaded<Ts...>(ts...);
}