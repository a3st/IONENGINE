// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/vector.hpp"

namespace ionengine::math
{
    template <typename Type>
    struct Mat
    {
        Type _00, _01, _02, _03;
        Type _10, _11, _12, _13;
        Type _20, _21, _22, _23;
        Type _30, _31, _32, _33;

        Mat()
            : _00(0), _01(0), _02(0), _03(0), _10(0), _11(0), _12(0), _13(0), _20(0), _21(0), _22(0), _23(0), _30(0),
              _31(0), _32(0), _33(0)
        {
        }

        Mat(Type const _00, Type const _01, Type const _02, Type const _03, Type const _10, Type const _11,
            Type const _12, Type const _13, Type const _20, Type const _21, Type const _22, Type const _23,
            Type const _30, Type const _31, Type const _32, Type const _33)
            : _00(_00), _01(_01), _02(_02), _03(_03), _10(_10), _11(_11), _12(_12), _13(_13), _20(_20), _21(_21),
              _22(_22), _23(_23), _30(_30), _31(_31), _32(_32), _33(_33)
        {
        }

        Mat(Mat const& other)
            : _00(other._00), _01(other._01), _02(other._02), _03(other._03), _10(other._10), _11(other._11),
              _12(other._12), _13(other._13), _20(other._20), _21(other._21), _22(other._22), _23(other._23),
              _30(other._30), _31(other._31), _32(other._32), _33(other._33)
        {
        }

        Mat(Mat&& other) noexcept
            : _00(other._00), _01(other._01), _02(other._02), _03(other._03), _10(other._10), _11(other._11),
              _12(other._12), _13(other._13), _20(other._20), _21(other._21), _22(other._22), _23(other._23),
              _30(other._30), _31(other._31), _32(other._32), _33(other._33)
        {
        }

        auto operator=(Mat const& other) -> Mat&
        {
            _00 = other._00;
            _01 = other._01;
            _02 = other._02;
            _03 = other._03;
            _10 = other._10;
            _11 = other._11;
            _12 = other._12;
            _13 = other._13;
            _20 = other._20;
            _21 = other._21;
            _22 = other._22;
            _23 = other._23;
            _30 = other._30;
            _31 = other._31;
            _32 = other._32;
            _33 = other._33;
            return *this;
        }

        auto operator=(Mat&& other) -> Mat&
        {
            _00 = other._00;
            _01 = other._01;
            _02 = other._02;
            _03 = other._03;
            _10 = other._10;
            _11 = other._11;
            _12 = other._12;
            _13 = other._13;
            _20 = other._20;
            _21 = other._21;
            _22 = other._22;
            _23 = other._23;
            _30 = other._30;
            _31 = other._31;
            _32 = other._32;
            _33 = other._33;
            return *this;
        }

        auto data() const -> Type const*
        {
            return &_00;
        }

        auto size() const -> size_t
        {
            return 16;
        }

        auto transpose() -> Mat&
        {
            Mat mat = *this;
            _00 = mat._00;
            _01 = mat._10;
            _02 = mat._20;
            _03 = mat._30;
            _10 = mat._01;
            _11 = mat._11;
            _12 = mat._21;
            _13 = mat._31;
            _20 = mat._02;
            _21 = mat._12;
            _22 = mat._22;
            _23 = mat._32;
            _30 = mat._03;
            _31 = mat._13;
            _32 = mat._23;
            _33 = mat._33;
            return *this;
        }

        auto inverse() -> Mat&
        {
            Type n11 = _00, n12 = _10, n13 = _20, n14 = _30;
            Type n21 = _01, n22 = _11, n23 = _21, n24 = _31;
            Type n31 = _02, n32 = _12, n33 = _22, n34 = _32;
            Type n41 = _03, n42 = _13, n43 = _23, n44 = _33;

            Type t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 +
                       n22 * n33 * n44;
            Type t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 -
                       n12 * n33 * n44;
            Type t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 +
                       n12 * n23 * n44;
            Type t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 -
                       n12 * n23 * n34;

            Type det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
            Type idet = static_cast<Type>(1.0f / det);

            _00 = t11 * idet;
            _01 = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 -
                   n21 * n33 * n44) *
                  idet;
            _02 = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 +
                   n21 * n32 * n44) *
                  idet;
            _03 = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 -
                   n21 * n32 * n43) *
                  idet;
            _10 = t12 * idet;
            _11 = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 +
                   n11 * n33 * n44) *
                  idet;
            _12 = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 -
                   n11 * n32 * n44) *
                  idet;
            _13 = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 +
                   n11 * n32 * n43) *
                  idet;
            _20 = t13 * idet;
            _21 = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 -
                   n11 * n23 * n44) *
                  idet;
            _22 = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 +
                   n11 * n22 * n44) *
                  idet;
            _23 = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 -
                   n11 * n22 * n43) *
                  idet;
            _30 = t14 * idet;
            _31 = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 +
                   n11 * n23 * n34) *
                  idet;
            _32 = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 -
                   n11 * n22 * n34) *
                  idet;
            _33 = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 +
                   n11 * n22 * n33) *
                  idet;
            return *this;
        };

        auto operator*(Mat const& other) const -> Mat
        {
            return Mat{_00 * other._00 + _01 * other._10 + _02 * other._20 + _03 * other._30,
                       _00 * other._01 + _01 * other._11 + _02 * other._21 + _03 * other._31,
                       _00 * other._02 + _01 * other._12 + _02 * other._22 + _03 * other._32,
                       _00 * other._03 + _01 * other._13 + _02 * other._23 + _03 * other._33,
                       _10 * other._00 + _11 * other._10 + _12 * other._20 + _13 * other._30,
                       _10 * other._01 + _11 * other._11 + _12 * other._21 + _13 * other._31,
                       _10 * other._02 + _11 * other._12 + _12 * other._22 + _13 * other._32,
                       _10 * other._03 + _11 * other._13 + _12 * other._23 + _13 * other._33,
                       _20 * other._00 + _21 * other._10 + _22 * other._20 + _23 * other._30,
                       _20 * other._01 + _21 * other._11 + _22 * other._21 + _23 * other._31,
                       _20 * other._02 + _21 * other._12 + _22 * other._22 + _23 * other._32,
                       _20 * other._03 + _21 * other._13 + _22 * other._23 + _23 * other._33,
                       _30 * other._00 + _31 * other._10 + _32 * other._20 + _33 * other._30,
                       _30 * other._01 + _31 * other._11 + _32 * other._21 + _33 * other._31,
                       _30 * other._02 + _31 * other._12 + _32 * other._22 + _33 * other._32,
                       _30 * other._03 + _31 * other._13 + _32 * other._23 + _33 * other._33};
        }

        auto operator*(Vec4<Type> const& other) const -> Mat
        {
            return Mat{_00 * other.x, _01 * other.y, _02 * other.z, _03 * other.w, _10 * other.x, _11 * other.y,
                       _12 * other.z, _13 * other.w, _20 * other.x, _21 * other.y, _22 * other.z, _23 * other.w,
                       _30 * other.x, _31 * other.y, _32 * other.z, _33 * other.w};
        }

        auto operator*(Type const other) const -> Mat
        {
            return Mat{_00 * other, _01 * other, _02 * other, _03 * other, _10 * other, _11 * other,
                       _12 * other, _13 * other, _20 * other, _21 * other, _22 * other, _23 * other,
                       _30 * other, _31 * other, _32 * other, _33 * other};
        }

        auto operator+(Mat const& other) const -> Mat
        {
            return Mat{_00 + other.m00, _01 + other._01, _02 + other._02, _03 + other._03,
                       _10 + other._10, _11 + other._11, _12 + other._12, _13 + other._13,
                       _20 + other._20, _21 + other._21, _22 + other._22, _23 + other._23,
                       _30 + other._30, _31 + other._31, _32 + other._32, _33 + other._33};
        }

        auto operator-(Mat const& other) const -> Mat
        {
            return Mat{_00 - other._00, _01 - other._01, _02 - other._02, _03 - other._03,
                       _10 - other._10, _11 - other._11, _12 - other._12, _13 - other._13,
                       _20 - other._20, _21 - other._21, _22 - other._22, _23 - other._23,
                       _30 - other._30, _31 - other._31, _32 - other._32, _33 - other._33};
        }

        static auto identity() -> Mat
        {
            Mat mat{};
            mat._00 = 1;
            mat._11 = 1;
            mat._22 = 1;
            mat._33 = 1;
            return mat;
        }

        static auto translate(Vec3<Type> const& position) -> Mat
        {
            auto mat = Mat<Type>::identity();
            mat._30 = position.x;
            mat._31 = position.y;
            mat._32 = position.z;
            return mat;
        }

        static auto scale(Vec3<Type> const& scale) -> Mat
        {
            auto mat = Mat<Type>::identity();
            mat._00 = scale.x;
            mat._11 = scale.y;
            mat._22 = scale.z;
            return mat;
        }

        static auto perspectiveRH(Type const fovy, Type const aspect, Type const nearDst, Type const farDst) -> Mat
        {
            Mat mat{};
            mat._00 = static_cast<Type>(1) / (std::tan(fovy / static_cast<Type>(2)) * aspect);
            mat._11 = static_cast<Type>(1) / std::tan(fovy / static_cast<Type>(2));
            mat._22 = farDst / (nearDst - farDst);
            mat._23 = -static_cast<Type>(1);
            mat._32 = (farDst * nearDst) / (nearDst - farDst);
            return mat;
        }

        static auto orthographicRH(Type const left, Type const right, Type const bottom, Type const top,
                                   Type const nearDst, Type const farDst) -> Mat
        {
            auto mat = Mat<Type>::identity();
            mat._00 = static_cast<Type>(2) / (right - left);
            mat._11 = static_cast<Type>(2) / (top - bottom);
            mat._22 = static_cast<Type>(1) / (nearDst - farDst);
            mat._30 = -(right + left) / (right - left);
            mat._31 = -(top + bottom) / (top - bottom);
            mat._32 = nearDst / (nearDst - farDst);
            return mat;
        }

        static auto lookAtRH(Vec3<Type> const& eye, Vec3<Type> const& center, Vec3<Type> const& up) -> Mat
        {
            Vec3<Type> f = (eye - center).normalize();
            Vec3<Type> r = Vec3(up).cross(f).normalize();
            Vec3<Type> u = Vec3(f).cross(r);

            auto mat = Mat<Type>::identity();
            mat._00 = r.x;
            mat._10 = r.y;
            mat._20 = r.z;
            mat._01 = u.x;
            mat._11 = u.y;
            mat._21 = u.z;
            mat._02 = f.x;
            mat._12 = f.y;
            mat._22 = f.z;
            mat._30 = -r.dot(eye);
            mat._31 = -u.dot(eye);
            mat._32 = -f.dot(eye);
            return mat;
        }

        auto operator==(Mat const& other) const -> bool
        {
            return std::make_tuple(_00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33) ==
                   std::make_tuple(other._00, other._01, other._02, other._03, other._10, other._11, other._12,
                                   other._13, other._20, other._21, other._22, other._23, other._30, other._31,
                                   other._32, other._33);
        }
    };

    using Matf = Mat<float>;
    using Matd = Mat<double>;
} // namespace ionengine::math

template <typename Type>
struct std::hash<ionengine::math::Mat<Type>>
{
    auto operator()(ionengine::math::Mat<Type> const& other) const -> size_t
    {
        return std::hash<Type>()(other._00) ^ std::hash<Type>()(other._01) ^ std::hash<Type>()(other._02) ^
               std::hash<Type>()(other._03) ^ std::hash<Type>()(other._10) ^ std::hash<Type>()(other._11) ^
               std::hash<Type>()(other._12) ^ std::hash<Type>()(other._13) ^ std::hash<Type>()(other._20) ^
               std::hash<Type>()(other._21) ^ std::hash<Type>()(other._22) ^ std::hash<Type>()(other._23) ^
               std::hash<Type>()(other._30) ^ std::hash<Type>()(other._31) ^ std::hash<Type>()(other._32) ^
               std::hash<Type>()(other._33);
    }
};