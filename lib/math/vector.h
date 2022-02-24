// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<typename Type>
struct Vector2 {
	
	Type x;
	Type y;

	Vector2() = default;

	Vector2(Type const _x, Type const _y) : x(_x), y(_y) { }

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

	const Type* data() const { return &x; }

	size_t size() const { return 2; }

	Vector2& normalize() {

        Type inverse = 1.0 / length();
	    x = x * inverse;
	    y = y * inverse;
		return *this;
    }

	Type length() const { 
		
		return std::sqrt(x * x + y * y); 
	}

	Vector2 operator*(Type const other) const {

		return Vector2 { x * other, y * other };
	}

	Vector2 operator-(Vector2 const& other) const {

		return Vector2 { x - other.x, y - other.y };
	}

	Vector2 operator-(Type const other) const {

		return Vector2 { x - other, y - other };
	}

	Vector2 operator-() const {

		return Vector2 { -x, -y };
	}

	Vector2 operator+(Vector2 const& other) const {

		return Vector2 { x + other.x, y + other.y };
	}

	Vector2 operator+(Type const other) const {

		return Vector2 { x + other, y + other };
	}

	Vector2 operator/(Type const other) const {

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

template<typename Type>
struct Vector3 {

	Type x;
	Type y;
	Type z;

	Vector3() = default;

	Vector3(const Type _x, const Type _y, const Type _z) : x(_x), y(_y), z(_z) {

    }

	Vector3(Vector3 const& other) : x(other.x), y(other.y), z(other.z) {

    }

	Vector3(Vector3&& other) noexcept : x(other.x), y(other.y), z(other.z) {

    }

	Vector3& operator=(Vector3 const& other) {

        x = other.x;
	    y = other.y;
	    z = other.z;
	    return *this;
    }

	Vector3& operator=(Vector3&& other) noexcept {

        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

	const Type* data() const { return &x; }

	size_t size() const { return 3; }

	Vector3& normalize() {
		
        Type inverse = 1.0f / length();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
		return *this;
    }

	Type length() const {

		return std::sqrt(x * x + y * y + z * z);
	}
    
	Vector3 operator*(Type const other) const {

		return Vector3 { x * other, y * other, z * other };
	}

	Vector3 operator-(Vector3 const& other) const {

		return Vector3 { x - other.x, y - other.y, z - other.z };
	}

	Vector3 operator-(Type const other) const {

		return Vector3 { x - other, y - other, z - other };
	}

	Vector3 operator-() const {

		return Vector3 { -x, -y, -z };
	}

	Vector3 operator+(Vector3 const& other) const {

		return Vector3 { x + other.x, y + other.y, z + other.z };
	}

	Vector3 operator+(Type const other) const {

		return Vector3 { x + other, y + other, z + other };
	}

	Vector3 operator/(Type const other) const {

		return Vector3 { x / other, y / other, z / other };
	}

	bool operator==(Vector3 const& other) const {

		return std::tie(x, y, z) == std::tie(other.x, other.y, other.z);
	}

	bool operator!=(Vector3 const& other) const {

		return std::tie(x, y, z) != std::tie(other.x, other.y, other.z);
	}

	Type dot(Vector3 const& other) const {

		return x * other.x + y * other.y + z * other.z;
	}

	Vector3& cross(Vector3 const& other) {

		x = y * other.z - z * other.y;
		y = z * other.x - x * other.z;
		z = x * other.y - y * other.x;
		return *this;
	}
};

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

template<typename Type>
struct Vector4 {

	Type x;
	Type y;
	Type z;
	Type w;

	Vector4() = default;

	Vector4(const Type _x, const Type _y, const Type _z, const Type _w) : x(_x), y(_y), z(_z), w(_w) {

    }

	Vector4(Vector4 const& other) : x(other.x), y(other.y), z(other.z), w(other.w) {

    }

	Vector4(Vector4&& other) noexcept : x(other.x), y(other.y), z(other.z), w(other.w) {

    }

	Vector4& operator=(Vector4 const& other) {

        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

	Vector4& operator=(Vector4&& other) noexcept {

        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

	const Type* data() const { return &x; }

	inline size_t size() const { return 4; }

	Vector4& normalize() {

        Type inverse = 1.0f / length();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
        w = w * inverse;
		return *this;
    }

	Type length() const {

		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	Vector4 operator*(Type const other) const {

		return Vector4 { x * other, y * other, z * other, w * other };
	}

	Vector4 operator-(Vector4 const& other) const {

		return Vector4 { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	Vector4 operator-(Type const other) const {

		return Vector4 { x - other, y - other, z - other, w - other };
	}

	Vector4 operator-() const {

		return Vector4 { -x, -y, -z, -w };
	}

	Vector4 operator+(Vector4 const& other) const {

		return Vector4 { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	Vector4 operator+(Type const other) const {

		return Vector4 { x + other, y + other, z + other, w + other };
	}

	Vector4 operator/(Type const other) const {

		return Vector4 { x / other, y / other, z / other, w / other };
	}

	bool operator==(Vector4 const& other) const {

		return std::tie(x, y, z) == std::tie(other.x, other.y, other.z, other.w);
	}

	bool operator!=(Vector4 const& other) const {

		return std::tie(x, y, z) != std::tie(other.x, other.y, other.z, other.w);
	}
};

using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;

}
