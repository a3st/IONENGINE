// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace lmath {

template<typename T>
struct Vector4 {

	T x, y, z, w;

	Vector4() : x(0), y(0), z(0), w(0) {

    }

	Vector4(const T _x, const T _y, const T _z, const T _w) : x(_x), y(_y), z(_z), w(_w) {

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

	inline const T* GetData() const {

		return &x;
	}

	inline size_t GetSize() const {

		return 4;
	}

	void Normalize() {

        T inverse = 1 / GetLength();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
        w = w * inverse;
    }

	inline T GetLength() const {

		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	inline Vector4 operator*(const T rhs) const {

		return Vector4 { x * rhs, y * rhs, z * rhs, w * rhs };
	}

	inline Vector4 operator-(const Vector4& rhs) const {

		return Vector4 { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
	}

	inline Vector4 operator-(const T rhs) const {

		return Vector4 { x - rhs, y - rhs, z - rhs, w - rhs };
	}

	inline Vector4 operator-() const {

		return Vector4 { -x, -y, -z, -w };
	}

	inline Vector4 operator+(const Vector4& rhs) const {

		return Vector4 { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
	}

	inline Vector4 operator+(const T rhs) const {

		return Vector4 { x + rhs, y + rhs, z + rhs, w + rhs };
	}

	inline Vector4 operator/(const T rhs) const {

		return Vector4 { x / rhs, y / rhs, z / rhs, w / rhs };
	}

	inline bool operator==(const Vector4& rhs) const {

		return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
	}

	inline bool operator!=(const Vector4& rhs) const {

		return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
	}
};

using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;

}