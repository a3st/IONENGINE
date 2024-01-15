// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <locale>
#include <format>
#include <random>
#include <limits>
#include <functional>
#include <exception>
#include <cassert>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <numeric>
#include <memory>
#include <array>
#include <future>
#include <queue>
#include <stack>
#include <map>
#include <string>
#include <variant>
#include <span>
#include <string_view>
#include <charconv>
#include <ranges>
#include <tuple>
#include <set>
#include <unordered_set>
#include <semaphore>
#include <regex>
#include <spanstream>

#ifndef DECLARE_ENUM_CLASS_BIT_FLAG
#define DECLARE_ENUM_CLASS_BIT_FLAG(EnumClass) \
inline EnumClass operator|(const EnumClass lhs, const EnumClass rhs) { \
	return static_cast<EnumClass>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)); \
} \
inline bool operator&(const EnumClass lhs, const EnumClass rhs) { \
	return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs); \
}
#endif // DECLARE_ENUM_CLASS_BIT_FLAG

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template <class... Ts>
inline auto make_visitor(Ts... ts) {

    return overloaded<Ts...>(ts...);
}