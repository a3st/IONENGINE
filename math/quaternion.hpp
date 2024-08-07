// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/matrix.hpp"

namespace ionengine::math
{
    template <typename Type>
    struct Quaternion
    {
        Type x;
        Type y;
        Type z;
        Type w;

        Quaternion() = default;

        Quaternion(Type const x, Type const y, Type const z, Type const w) : x(x), y(y), z(z), w(w)
        {
        }

        Quaternion(Quaternion const& other) : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        Quaternion(Quaternion&& other) noexcept : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        Quaternion& operator=(Quaternion const& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }

        Quaternion& operator=(Quaternion&& other) noexcept
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }

        Type const* data() const
        {
            return &x;
        }

        size_t size() const
        {
            return 4;
        }

        Quaternion& normalize()
        {
            Type inverse = 1.0 / length();
            x = x * inverse;
            y = y * inverse;
            z = z * inverse;
            w = w * inverse;
            return *this;
        }

        Type length() const
        {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        Matrix<Type> toMatrix() const
        {
            Type xy = x * y;
            Type xz = x * z;
            Type xw = x * w;
            Type yz = y * z;
            Type yw = y * w;
            Type zw = z * w;
            Type xx = x * x;
            Type yy = y * y;
            Type zz = z * z;

            auto mat = Matrix<Type>::identity();
            mat.m00 = 1.0f - 2.0f * (yy + zz);
            mat.m01 = 2.0f * (xy - zw);
            mat.m02 = 2.0f * (xz + yw);
            mat.m03 = 0.0f;
            mat.m10 = 2.0f * (xy + zw);
            mat.m11 = 1.0f - 2.0f * (xx + zz);
            mat.m12 = 2.0f * (yz - xw);
            mat.m13 = 0.0f;
            mat.m20 = 2.0f * (xz - yw);
            mat.m21 = 2.0f * (yz + xw);
            mat.m22 = 1.0f - 2.0f * (xx + yy);
            return mat;
        }

        void angleAxis(Type& angle, Vector3<Type>& axis) const
        {
            Quaternion quat{};
            quat = *this;

            if (w > 1)
            {
                quat.normalize();
            }

            *angle = static_cast<Type>(2 * std::acos(quat.w) * 180.0f / std::numbers::pi);
            Type s = std::sqrt(1 - quat.w * quat.w);

            if (s < 0.001)
            {
                axis->x = quat.x;
                axis->y = quat.y;
                axis->z = quat.z;
            }
            else
            {
                axis->x = quat.x / s;
                axis->y = quat.y / s;
                axis->z = quat.z / s;
            }
        }

        static Quaternion<Type> fromAngleAxis(Type const angle, Vector3<Type> const& axis)
        {
            auto quat = Quaternion<Type>{};

            Type rotAngle = static_cast<Type>(angle * std::numbers::pi / 180.0f);
            Type rotSin = std::sin(rotAngle / 2);
            Vector3<Type> normAxis = Vector3<Type>(axis).normalize();

            quat.x = normAxis.x * rotSin;
            quat.y = normAxis.y * rotSin;
            quat.z = normAxis.z * rotSin;
            quat.w = std::cos(rotAngle / 2);
            return quat;
        }

        Quaternion operator*(Quaternion const& other) const
        {
            Quaternion quat;
            quat.x = w * other.x + x * other.w + y * other.z - z * other.y;
            quat.y = w * other.y - x * other.z + y * other.w + z * other.x;
            quat.z = w * other.z + x * other.y - y * other.x + z * other.w;
            quat.w = w * other.w - x * other.x - y * other.y - z * other.z;
            return quat;
        }

        Vector3<Type> operator*(Vector3<Type> const& other) const
        {
            Vector3<Type> vec(x, y, z);
            Vector3<Type> uv(Vector3<Type>(vec).cross(other));
            Vector3<Type> uuv(Vector3<Type>(vec).cross(uv));
            return other + ((uv * w) + uuv) * static_cast<Type>(2);
        }

        Quaternion operator*(Type const other) const
        {
            return Quaternion{x * other, y * other, z * other, w * other};
        }

        Quaternion operator+(Quaternion const& other) const
        {
            return Quaternion{x + other.x, y + other.y, z + other.z, w + other.w};
        }

        Quaternion operator-(Quaternion const& other) const
        {
            return Quaternion{x - other.x, y - other.y, z - other.z, w - other.w};
        }

        bool operator==(Quaternion const& other) const
        {
            return std::tie(x, y, z, w) == std::tie(other.x, other.y, other.z, other.w);
        }

        bool operator!=(Quaternion const& other) const
        {
            return std::tie(x, y, z, w) != std::tie(other.x, other.y, other.z, other.w);
        }

        static Quaternion euler(Type const x, Type const y, Type const z)
        {
            auto quat = Quaternion{};

            Type rotx, roty, rotz;
            rotx = static_cast<Type>(x * std::numbers::pi / 180.0f);
            roty = static_cast<Type>(y * std::numbers::pi / 180.0f);
            rotz = static_cast<Type>(z * std::numbers::pi / 180.0f);

            Type sinx, siny, sinz, cosx, cosy, cosz;
            sinx = std::sin(rotx / 2);
            siny = std::sin(roty / 2);
            sinz = std::sin(rotz / 2);
            cosx = std::cos(rotx / 2);
            cosy = std::cos(roty / 2);
            cosz = std::cos(rotz / 2);

            quat.x = sinx * cosy * cosz + cosx * siny * sinz;
            quat.y = cosx * siny * cosz - sinx * cosy * sinz;
            quat.z = cosx * cosy * sinz - sinx * siny * cosz;
            quat.w = cosx * cosy * cosz + sinx * siny * sinz;

            return quat;
        }
    };

    using Quaternionf = Quaternion<float>;
    using Quaterniond = Quaternion<double>;
} // namespace ionengine::math