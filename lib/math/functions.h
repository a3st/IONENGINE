// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math {
	
constexpr double pi = M_PI;
constexpr double deg2rad = M_PI / 180.0f;
constexpr double rad2deg = 180.0f / M_PI;

template<typename T>
T sqrt(const T& value) {
	if constexpr (std::is_same<T, float>::value) {
		return std::sqrtf(value);
	} else if constexpr(std::is_same<T, double>::value) {
		return std::sqrt(value);
	} else {
		static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "sqrt should be float or double type");
	}
}

template<typename T>
T sin(const T& value) {
	if constexpr (std::is_same<T, float>::value) {
		return std::sinf(value);
	} else if constexpr(std::is_same<T, double>::value) {
		return std::sin(value);
	} else {
		static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "sin should be float or double type");
	}
}

template<typename T>
T cos(const T& value) {
	if constexpr (std::is_same<T, float>::value) {
		return std::cosf(value);
	} else if constexpr(std::is_same<T, double>::value) {
		return std::cos(value);
	} else {
		static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "cos should be float or double type");
	}
}

template<typename T>
T tan(const T& value) {
	if constexpr (std::is_same<T, float>::value) {
		return std::tanf(value);
	} else if constexpr(std::is_same<T, double>::value) {
		return std::tan(value);
	} else {
		static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "tan should be float or double type");
	}
}

template<typename T>
T atan(const T& value) {
	if constexpr (std::is_same<T, float>::value) {
		return std::atanf(value);
	} else if constexpr(std::is_same<T, double>::value) {
		return std::atan(value);
	} else {
		static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "atan should be float or double type");
	}
}

template<typename T>
T pow(const T& value, const T& power) {
	if constexpr (std::is_same<T, float>::value) {
		return std::powf(value, power);
	} else if constexpr(std::is_same<T, double>::value) {
		return std::pow(value, power);
	} else {
		static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "pow should be float or double type");
	}
}

}