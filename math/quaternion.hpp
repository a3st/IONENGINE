// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/matrix.hpp"

namespace ionengine::math
{
    template <typename Type>
    struct Quat
    {
        Type x;
        Type y;
        Type z;
        Type w;

        Quat() : x(0), y(0), z(0), w(0)
        {
        }

        Quat(Type const x, Type const y, Type const z, Type const w) : x(x), y(y), z(z), w(w)
        {
        }

        Quat(Quat const& other) : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        Quat(Quat&& other) noexcept : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        auto operator=(Quat const& other) -> Quat&
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }

        auto operator=(Quat&& other) -> Quat&
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }

        auto data() const -> Type const*
        {
            return &x;
        }

        auto size() const -> size_t
        {
            return 4;
        }

        auto normalize() -> Quat&
        {
            Type inverse = 1.0 / this->length();
            x = x * inverse;
            y = y * inverse;
            z = z * inverse;
            w = w * inverse;
            return *this;
        }

        auto length() const -> Type
        {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        auto toMat() const -> Mat4<Type>
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

            auto mat = Mat4<Type>::identity();
            mat._00 = 1.0f - 2.0f * (yy + zz);
            mat._01 = 2.0f * (xy - zw);
            mat._02 = 2.0f * (xz + yw);
            mat._03 = 0.0f;
            mat._10 = 2.0f * (xy + zw);
            mat._11 = 1.0f - 2.0f * (xx + zz);
            mat._12 = 2.0f * (yz - xw);
            mat._13 = 0.0f;
            mat._20 = 2.0f * (xz - yw);
            mat._21 = 2.0f * (yz + xw);
            mat._22 = 1.0f - 2.0f * (xx + yy);
            return mat;
        }

        auto toAngleAxis(Type& angle, Vec3<Type>& axis) const -> void
        {
            Quat quat = *this;

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

        static auto fromAngleAxis(Type const angle, Vec3<Type> const& axis) -> Quat
        {
            Quat quat{};

            Type rotAngle = static_cast<Type>(angle * std::numbers::pi / 180.0f);
            Type rotSin = std::sin(rotAngle / 2);
            auto normAxis = Vec3<Type>(axis).normalize();

            quat.x = normAxis.x * rotSin;
            quat.y = normAxis.y * rotSin;
            quat.z = normAxis.z * rotSin;
            quat.w = std::cos(rotAngle / 2);
            return quat;
        }

        auto operator*(Quat const& other) const -> Quat
        {
            return Quat{w * other.x + x * other.w + y * other.z - z * other.y,
                        w * other.y - x * other.z + y * other.w + z * other.x,
                        w * other.z + x * other.y - y * other.x + z * other.w,
                        w * other.w - x * other.x - y * other.y - z * other.z};
        }

        auto operator*(Vec3<Type> const& other) const -> Vec3<Type>
        {
            Vec3<Type> vec(x, y, z);
            Vec3<Type> uv(Vec3<Type>(vec).cross(other));
            Vec3<Type> uuv(Vec3<Type>(vec).cross(uv));
            return other + ((uv * w) + uuv) * static_cast<Type>(2);
        }

        auto operator*(Type const other) const -> Quat
        {
            return Quat{x * other, y * other, z * other, w * other};
        }

        auto operator+(Quat const& other) const -> Quat
        {
            return Quat{x + other.x, y + other.y, z + other.z, w + other.w};
        }

        auto operator-(Quat const& other) const -> Quat
        {
            return Quat{x - other.x, y - other.y, z - other.z, w - other.w};
        }

        auto operator==(Quat const& other) const -> bool
        {
            return std::make_tuple(x, y, z, w) == std::make_tuple(other.x, other.y, other.z, other.w);
        }

        static auto euler(Type const x, Type const y, Type const z) -> Quat
        {
            Quat quat{};

            Type rotx = static_cast<Type>(x * std::numbers::pi / 180.0f);
            Type roty = static_cast<Type>(y * std::numbers::pi / 180.0f);
            Type rotz = static_cast<Type>(z * std::numbers::pi / 180.0f);

            Type sinx = std::sin(rotx / 2);
            Type siny = std::sin(roty / 2);
            Type sinz = std::sin(rotz / 2);

            Type cosx = std::cos(rotx / 2);
            Type cosy = std::cos(roty / 2);
            Type cosz = std::cos(rotz / 2);

            quat.x = sinx * cosy * cosz + cosx * siny * sinz;
            quat.y = cosx * siny * cosz - sinx * cosy * sinz;
            quat.z = cosx * cosy * sinz - sinx * siny * cosz;
            quat.w = cosx * cosy * cosz + sinx * siny * sinz;
            return quat;
        }
    };

    using Quatf = Quat<float>;
    using Quatd = Quat<double>;
} // namespace ionengine::math

template <typename Type>
struct std::hash<ionengine::math::Quat<Type>>
{
    auto operator()(ionengine::math::Quat<Type> const& other) const -> size_t
    {
        return std::hash<Type>()(other.x) ^ std::hash<Type>()(other.y) ^ std::hash<Type>()(other.z) ^
               std::hash<Type>()(other.w);
    }
};