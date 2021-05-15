// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math {

template<typename T>
struct Quaternion {
	T x, y, z, w;

	Quaternion() : x(0), y(0), z(0), w(0) {
    }

	Quaternion(const T x_, const T y_, const T z_, const T w_) : x(x_), y(y_), z(z_), w(w_) {
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

	void normalize() {
        T inverse = 1 / this->length();
        x = x * inverse;
        y = y * inverse;
        z = z * inverse;
        w = w * inverse;
    }

	Matrix to_matrix() const {
        T sqw = w * w;
        T sqx = x * x;
        T sqy = y * y;
        T sqz = z * z;
        T inverse = 1 / (sqx + sqy + sqz + sqw);

        Matrix mat = Matrix::identity();
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

    T length() const {
		return math::sqrt(x * x + y * y + z * z + w * w);
	}

	void to_angle_axis(T* angle, Vector3* axis) const {
        Quaternion quat;
        quat = *this;

        if (w > 1) {
            quat.normalize();
        }

        *angle = static_cast<T>(2 * math::acos(quat.w) * math::rad2deg);
        T s = math::sqrt(1 - quat.w * quat.w);

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

	Quaternion operator*(const value_type rhs) const {
		return Quaternion(x * rhs, y * rhs, z * rhs, w * rhs);
	}

	Quaternion operator+(const Quaternion& rhs) const {
		return Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	Quaternion operator-(const Quaternion& rhs) const {
		return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	bool operator!=(const Quaternion& rhs) const {
		return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
	}

	bool operator==(const Quaternion& rhs) const {
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
	}

    static Quaternion euler(const T x, const T y, const T z) {
        T rotx, roty, rotz;
        rotx = static_cast<T>(x * math::deg2Rad);
        roty = static_cast<T>(y * math::deg2Rad);
        rotz = static_cast<T>(z * math::deg2Rad);

        T sinx, siny, sinz, cosx, cosy, cosz;
        sinx = math::sin(rotx / 2);
        siny = math::sin(roty / 2);
        sinz = math::sin(rotz / 2);
        cosx = math::cos(rotx / 2);
        cosy = math::cos(roty / 2);
        cosz = math::cos(rotz / 2);

        Quaternion quat;
        quat.w = cosx * cosy * cosz + sinx * siny * sinz;
        quat.x = sinx * cosy * cosz + cosx * siny * sinz;
        quat.y = cosx * siny * cosz - sinx * cosy * sinz;
        quat.z = cosx * cosy * sinz - sinx * siny * cosz;
        return quat;
    }

	static Quaternion angle_axis(const T angle, const Vector3& axis) {
        T rot_angle = static_cast<T>(angle * math::deg2rad);
	    T rot_sin = math::sin(rot_angle / 2);
        Vector3 norm_axis = axis.normalize();
	    return Quaternion(norm_axis.x * rot_sin, norm_axis.y * rot_sin, norm_axis.z * rot_sin, math::cos(rot_angle / 2));
    }
};

using Fquaternion = Quaternion<float>;
using Dquaternion = Quaternion<double>;

}