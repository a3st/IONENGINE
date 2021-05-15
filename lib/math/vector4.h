// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math {

template<typename T>
struct Vector4 {
	T x, y, z, w;

	Vector4() : x(0), y(0), z(0), w(0) {
    }

	Vector4(const T x_, const T y_, const T z_, const T w_) : x(x_), y(y_), z(z_), w(w_) {
    }

	Vector4(const Vector4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {
    }

	Vector4(Vector4&& rhs) noexcept : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {
    }

	Vector4& operator=(const Vector4& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

	Vector4& operator=(Vector4&& rhs) noexcept {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

	const T* data() const {
		return &x;
	}

	usize size() const {
		return 4;
	}

	void normalize() {
        T inverse = 1 / this->length();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
        w = w * inverse;
    }

	T length() const {
		return math::sqrt(x * x + y * y + z * z + w * w);
	}

	Vector4 operator*(const T rhs) const {
		return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
	}

	Vector4 operator-(const Vector4& rhs) const {
		return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	Vector4 operator-(const T rhs) const {
		return Vector4(x - rhs, y - rhs, z - rhs, w - rhs);
	}

	Vector4 operator-() const {
		return Vector4(-x, -y, -z, -w);
	}

	Vector4 operator+(const Vector4& rhs) const {
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	Vector4 operator+(const T rhs) const {
		return Vector4(x + rhs, y + rhs, z + rhs, w + rhs);
	}

	Vector4 operator/(const T rhs) const {
		return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
	}

	bool operator==(const Vector4& rhs) const {
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
	}

	bool operator!=(const Vector4& rhs) const {
		return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
	}
};

using Fvector4 = Vector4<float>;
using Dvector4 = Vector4<double>;

}