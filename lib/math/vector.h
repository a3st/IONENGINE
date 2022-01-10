// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<typename T>
struct Vector2 {
	
	T x;
	T y;

	Vector2() = default;

	Vector2(T const _x, T const _y, T const _z) : x(_x), y(_y) { }

	Vector2(Vector2 const& other) : x(other.x), y(other.y) { }

	Vector2(Vector2&& other) noexcept : x(other.x), y(other.y) { }

	Vector2& operator=(Vector2 const& other) {

	    x = other.x;
	    y = other.y;
	    return *this;
    }

	Vector2& operator=(Vector2&& other) noexcept {

	    x = other.x;
	    y = other.y;
	    return *this;
    }

	const T* data() const { return &x; }

	size_t size() const { return 2; }

	Vector2& normalize() {

        T inverse = 1.0 / length();
	    x = x * inverse;
	    y = y * inverse;
		return *this;
    }

	T length() const { 
		
		return std::sqrt(x * x + y * y); 
	}

	Vector2 operator*(T const other) const {

		return Vector2 { x * other, y * other };
	}

	Vector2 operator-(Vector2 const& other) const {

		return Vector2 { x - other.x, y - other.y };
	}

	Vector2 operator-(T const other) const {

		return Vector2 { x - other, y - other };
	}

	Vector2 operator-() const {

		return Vector2 { -x, -y };
	}

	Vector2 operator+(Vector2 const& other) const {

		return Vector2 { x + other.x, y + other.y };
	}

	Vector2 operator+(T const other) const {

		return Vector2 { x + other, y + other };
	}

	Vector2 operator/(T const other) const {

		return Vector2 { x / other, y / other };
	}

	bool operator==(Vector2 const& other) const {

        return std::tie(x, y) == std::tie(other.x, other.y);
    }

	bool operator!=(Vector2 const& other) const {

        return std::tie(x, y) != std::tie(other.x, other.y);
    }
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

}

