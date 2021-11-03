// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace lmath {

template<typename T>
struct Vector2 {
	
	T x, y;

	Vector2() : x(0), y(0) {

    }

	Vector2(const T _x, const T _y, const T _z) : x(_x), y(_y) {

    }

	Vector2(const Vector2& rhs) : x(rhs.x), y(rhs.y) {

    }

	Vector2(Vector2&& rhs) noexcept : x(rhs.x), y(rhs.y) {

    }

	Vector2& operator=(const Vector2& rhs) {

	    x = rhs.x;
	    y = rhs.y;
	    return *this;
    }

	Vector2& operator=(Vector2&& rhs) noexcept {

	    x = rhs.x;
	    y = rhs.y;
	    return *this;
    }

	inline const T* GetData() const {

		return &x;
	}

	inline size_t GetSize() const {

		return 2;
	}

	void Normalize() {

        T inverse = 1.0f / GetLength();
	    x = x * inverse;
	    y = y * inverse;
    }

	inline T GetLength() const {

		return std::sqrt(x * x + y * y);
	}

	inline Vector2 operator*(const T rhs) const {

		return Vector2 { x * rhs, y * rhs };
	}

	inline Vector2 operator-(const Vector2& rhs) const {

		return Vector2 { x - rhs.x, y - rhs.y };
	}

	inline Vector2 operator-(const T rhs) const {

		return Vector2 { x - rhs, y - rhs };
	}

	inline Vector2 operator-() const {

		return Vector2 { -x, -y };
	}

	inline Vector2 operator+(const Vector2& rhs) const {

		return Vector2 { x + rhs.x, y + rhs.y };
	}

	inline Vector2 operator+(const T rhs) const {

		return Vector2 { x + rhs, y + rhs };
	}

	inline Vector2 operator/(const T rhs) const {

		return Vector2 { x / rhs, y / rhs };
	}

	inline bool operator==(const Vector2& rhs) const {

        return x == rhs.x && y == rhs.y;
    }

	inline bool operator!=(const Vector2& rhs) const {

        return x != rhs.x || y != rhs.y;
    }
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

}