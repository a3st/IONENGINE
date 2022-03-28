// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/matrix.h>

namespace ionengine::lib::math {

template<typename Type>
struct Quaternion {

	Type x;
    Type y;
    Type z;
    Type w;

	Quaternion() = default;

	Quaternion(T const _x, T const _y, T const _z, T const _w) : x(_x), y(_y), z(_z), w(_w) { }

	Quaternion(Quaternion const& other) : x(other.x), y(other.y), z(other.z), w(other.w) { }

	Quaternion(Quaternion&& other) noexcept : x(other.x), y(other.y), z(other.z), w(other.w) {

    }

	Quaternion& operator=(Quaternion const& other) {

        x = other.x;
	    y = other.y;
	    z = other.z;
	    w = other.w;
	    return *this;
    }

	Quaternion& operator=(Quaternion&& other) noexcept {

        x = other.x;
	    y = other.y;
	    z = other.z;
	    w = other.w;
	    return *this;
    }

    const Type* data() const { return &x; }

    size_t size() const { return 4; }

	Quaternion& normalize() {

        T inverse = 1.0 / length();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
        w = w * inverse;
        return *this;
    }

    Type length() const {

		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	Matrix<Type> matrix() const {

        Type sqw = w * w;
        Type sqx = x * x;
        Type sqy = y * y;
        Type sqz = z * z;
        Type inverse = 1 / (sqx + sqy + sqz + sqw);

        Matrix<Type> mat{}.identity();
        mat.m00 = (sqx - sqy - sqz + sqw) * inverse;
        mat.m11 = (-sqx + sqy - sqz + sqw) * inverse;
        mat.m22 = (-sqx - sqy + sqz + sqw) * inverse;
        mat.m10 = 2 * (x * y + z * w) * inverse;
        mat.m01 = 2 * (x * y - z * w) * inverse;
        mat.m20 = 2 * (x * z - y * w) * inverse;
        mat.m02 = 2 * (x * z + y * w) * inverse;
        mat.m21 = 2 * (y * z + x * w) * inverse;
        mat.m12 = 2 * (y * z - x * w) * inverse;
        return mat;
    }

	void angle_axis(Type& angle, Vector3<Type>& axis) const {

        Quaternion quat{};
        quat = *this;

        if (w > 1) {
            quat.Normalize();
        }

        *angle = static_cast<Type>(2 * std::acos(quat.w) * 180.0f / M_PI);
        Type s = std::sqrt(1 - quat.w * quat.w);

        if (s < 0.001) {
            axis->x = quat.x;
            axis->y = quat.y;
            axis->z = quat.z;
        } else {
            axis->x = quat.x / s;
            axis->y = quat.y / s;
            axis->z = quat.z / s;
        }
    }

    Quaternion& angle_axis(Type const angle, Vector3<Type> const& axis) {

        Type rot_angle = static_cast<Type>(angle * M_PI / 180.0f);
	    Type rot_sin = std::sin(rot_angle / 2);
        Vector3<Type> norm_axis = axis.Normalize();

        x = norm_axis.x * rot_sin;
        y = norm_axis.y * rot_sin;
        z = norm_axis.z * rot_sin;
        w = std::cos(rot_angle / 2);

	    return *this;
    }

	Quaternion operator*(Quaternion const& other) const {

        Quaternion quat;
	    quat.x = w * other.x + x * other.w + y * other.z - z * other.y;
	    quat.y = w * other.y - x * other.z + y * other.w + z * other.x;
	    quat.z = w * other.z + x * other.y - y * other.x + z * other.w;
	    quat.w = w * other.w - x * other.x - y * other.y - z * other.z;
	    return quat;
    }

	Quaternion operator*(Type const other) const {

		return Quaternion { x * other, y * other, z * other, w * other };
	}

	Quaternion operator+(Quaternion const& other) const {

		return Quaternion { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	Quaternion operator-(Quaternion const& other) const {

		return Quaternion { x - other.x, y - other.y, z - other.z, w - other.w };
	}

    bool operator==(Quaternion const& other) const {

		return std::tie(x, y, z, w) == std::tie(other.x, other.y, other.z, other.w);
	}

	bool operator!=(Quaternion const& other) const {

		return std::tie(x, y, z, w) != std::tie(other.x, other.y, other.z, other.w);
	}

    Quaternion& euler(Type const x, Type const y, Type const z) {

        Type rotx, roty, rotz;
        rotx = static_cast<T>(x * M_PI / 180.0f);
        roty = static_cast<T>(y * M_PI / 180.0f);
        rotz = static_cast<T>(z * M_PI / 180.0f);

        Type sinx, siny, sinz, cosx, cosy, cosz;
        sinx = std::sin(rotx / 2);
        siny = std::sin(roty / 2);
        sinz = std::sin(rotz / 2);
        cosx = std::cos(rotx / 2);
        cosy = std::cos(roty / 2);
        cosz = std::cos(rotz / 2);

        x = sinx * cosy * cosz + cosx * siny * sinz;
        y = cosx * siny * cosz - sinx * cosy * sinz;
        z = cosx * cosy * sinz - sinx * siny * cosz;
        w = cosx * cosy * cosz + sinx * siny * sinz;

        return *this;
    }
};

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

}