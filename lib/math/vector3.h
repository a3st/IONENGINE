// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math {

template<typename T>
struct Vector3 {
	T x, y, z;

	Vector3() : x(0), y(0), z(0) {
    }

	Vector3(const T x_, const T y_, const T z_) : x(x_), y(y_), z(z_) {
    }

	Vector3(const Vector3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {
    }

	Vector3(Vector3&& rhs) noexcept : x(rhs.x), y(rhs.y), z(rhs.z) {
    }

	Vector3& operator=(const Vector3& rhs) {
        x = rhs.x;
	    y = rhs.y;
	    z = rhs.z;
	    return *this;
    }

	Vector3& operator=(Vector3&& rhs) noexcept {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

	const T* data() const {
		return &x;
	}

	usize size() const {
		return 3;
	}

	void normalize() {
        T inverse = 1 / this->length();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
    }

	T length() const {
		return math::sqrt(x * x + y * y + z * z);
	}
    
	Vector3 operator*(const T rhs) const {
		return Vector3(x * rhs, y * rhs, z * rhs);
	}

	Vector3 operator-(const Vector3& rhs) const {
		return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vector3 operator-(const T rhs) const {
		return Vector3(x - rhs, y - rhs, z - rhs);
	}

	Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}

	Vector3 operator+(const Vector3& rhs) const {
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3 operator+(const T rhs) const {
		return Vector3(x + rhs, y + rhs, z + rhs);
	}

	Vector3 operator/(const T rhs) const {
		return Vector3(x / rhs, y / rhs, z / rhs);
	}

	bool operator==(const Vector3& rhs) const {
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
	}

	bool operator!=(const Vector3& rhs) const {
		return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
	}

	static T dot_product(const Vector3& lhs, const Vector3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	static Vector3 cross_product(const Vector3& lhs, const Vector3& rhs) {
		return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
	}
};

using Fvector3 = Vector3<float>;
using Dvector3 = Vector3<double>;

}