// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace lmath {

template<typename T>
struct Quaternion {

	T x, y, z, w;

	Quaternion() : x(0), y(0), z(0), w(0) {

    }

	Quaternion(const T _x, const T _y, const T _z, const T _w) : x(_x), y(_y), z(_z), w(_w) {

    }

	Quaternion(const Quaternion& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {

    }

	Quaternion(Quaternion&& rhs) noexcept : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {

    }

	Quaternion& operator=(const Quaternion& rhs) {

        x = rhs.x;
	    y = rhs.y;
	    z = rhs.z;
	    w = rhs.w;
	    return *this;
    }

	Quaternion& operator=(Quaternion&& rhs) noexcept {

        x = rhs.x;
	    y = rhs.y;
	    z = rhs.z;
	    w = rhs.w;
	    return *this;
    }

	void Normalize() {

        T inverse = 1 / GetLength();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
        w = w * inverse;
    }

	Matrix<T> ToMatrix() const {

        T sqw = w * w;
        T sqx = x * x;
        T sqy = y * y;
        T sqz = z * z;
        T inverse = 1 / (sqx + sqy + sqz + sqw);

        Matrix<T> mat = Matrix<T>::Identity();
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

    inline T GetLength() const {

		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	void ToAngleAxis(T* angle, Vector3<T>* axis) const {

        Quaternion quat{};
        quat = *this;

        if (w > 1) {
            quat.Normalize();
        }

        *angle = static_cast<T>(2 * std::acos(quat.w) * kRad2Deg);
        T s = std::sqrt(1 - quat.w * quat.w);

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

	Quaternion operator*(const Quaternion& rhs) const {

        Quaternion quat;
	    quat.x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
	    quat.y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
	    quat.z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
	    quat.w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
	    return quat;
    }

	inline Quaternion operator*(const T rhs) const {

		return Quaternion { x * rhs, y * rhs, z * rhs, w * rhs };
	}

	inline Quaternion operator+(const Quaternion& rhs) const {

		return Quaternion { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
	}

	inline Quaternion operator-(const Quaternion& rhs) const {

		return Quaternion { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
	}

    inline bool operator==(const Quaternion& rhs) const {

		return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
	}

	inline bool operator!=(const Quaternion& rhs) const {

		return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
	}

    static Quaternion Euler(const T x, const T y, const T z) {

        T rotx, roty, rotz;
        rotx = static_cast<T>(x * kDeg2Rad);
        roty = static_cast<T>(y * kDeg2Rad);
        rotz = static_cast<T>(z * kDeg2Rad);

        T sinx, siny, sinz, cosx, cosy, cosz;
        sinx = std::sin(rotx / 2);
        siny = std::sin(roty / 2);
        sinz = std::sin(rotz / 2);
        cosx = std::cos(rotx / 2);
        cosy = std::cos(roty / 2);
        cosz = std::cos(rotz / 2);

        Quaternion quat{};
        quat.w = cosx * cosy * cosz + sinx * siny * sinz;
        quat.x = sinx * cosy * cosz + cosx * siny * sinz;
        quat.y = cosx * siny * cosz - sinx * cosy * sinz;
        quat.z = cosx * cosy * sinz - sinx * siny * cosz;
        return quat;
    }

    static Quaternion AngleAxis(const T angle, const Vector3<T>& axis) {

        T rot_angle = static_cast<T>(angle * kDeg2Rad);
	    T rot_sin = std::sin(rot_angle / 2);
        Vector3<T> norm_axis = axis.Normalize();
	    return Quaternion { norm_axis.x * rot_sin, norm_axis.y * rot_sin, norm_axis.z * rot_sin, std::cos(rot_angle / 2) };
    }
};

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

}