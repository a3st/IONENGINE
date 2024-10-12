// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math
{
    template <typename Type>
    struct Vec2
    {
        Type x;
        Type y;

        Vec2() : x(0), y(0)
        {
        }

        Vec2(Type const x, Type const y) : x(x), y(y)
        {
        }

        Vec2(Vec2 const& other) : x(other.x), y(other.y)
        {
        }

        Vec2(Vec2&& other) : x(other.x), y(other.y)
        {
        }

        auto operator=(Vec2 const& other) -> Vector2&
        {
            x = other.x;
            y = other.y;
            return *this;
        }

        auto operator=(Vec2&& other) -> Vector2&
        {
            x = other.x;
            y = other.y;
            return *this;
        }

        auto data() const -> Type const*
        {
            return &x;
        }

        auto size() const -> size_t
        {
            return 2;
        }

        auto normalize() -> Vec2&
        {
            Type inverse = static_cast<Type>(1) / this->length();
            x = x * inverse;
            y = y * inverse;
            return *this;
        }

        auto length() const -> Type
        {
            return static_cast<Type>(std::sqrt(x * x + y * y));
        }

        auto operator*(Type const other) const -> Vec2
        {
            return Vec2{x * other, y * other};
        }

        auto operator-(Vec2 const& other) const -> Vec2
        {
            return Vec2{x - other.x, y - other.y};
        }

        auto operator-(Type const other) const -> Vec2
        {
            return Vec2{x - other, y - other};
        }

        auto operator-() const -> Vec2
        {
            return Vec2{-x, -y};
        }

        auto operator+(Vec2 const& other) const -> Vec2
        {
            return Vec2{x + other.x, y + other.y};
        }

        auto operator+(Type const other) const -> Vec2
        {
            return Vec2{x + other, y + other};
        }

        auto operator/(Type const other) const -> Vec2
        {
            return Vec2{x / other, y / other};
        }

        bool operator==(Vec2 const& other) const
        {
            return std::make_tuple(x, y) == std::make_tuple(other.x, other.y);
        }
    };

    using Vec2f = Vec2<float>;
    using Vec2d = Vec2<double>;

    template <typename Type>
    struct Vec3
    {
        Type x;
        Type y;
        Type z;

        Vec3() : x(0), y(0), z(0)
        {
        }

        Vec3(const Type x, const Type y, const Type z) : x(x), y(y), z(z)
        {
        }

        Vec3(Vec3 const& other) : x(other.x), y(other.y), z(other.z)
        {
        }

        Vec3(Vec3&& other) : x(other.x), y(other.y), z(other.z)
        {
        }

        auto operator=(Vec3 const& other) -> Vec3
        {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }

        auto operator=(Vec3&& other) -> Vec3
        {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }

        auto data() const -> Type const*
        {
            return &x;
        }

        auto size() const -> size_t
        {
            return 3;
        }

        auto normalize() -> Vec3
        {
            Type inverse = static_cast<Type>(1) / this->length();
            x = x * inverse;
            y = y * inverse;
            z = z * inverse;
            return *this;
        }

        auto length() const -> Type
        {
            return static_cast<Type>(std::sqrt(x * x + y * y + z * z));
        }

        auto operator*(Type const other) const -> Vec3
        {
            return Vec3{x * other, y * other, z * other};
        }

        auto operator-(Vec3 const& other) const -> Vec3
        {
            return Vec3{x - other.x, y - other.y, z - other.z};
        }

        auto operator-(Type const other) const -> Vec3
        {
            return Vec3{x - other, y - other, z - other};
        }

        auto operator-() const -> Vec3
        {
            return Vec3{-x, -y, -z};
        }

        auto operator+(Vec3 const& other) const -> Vec3
        {
            return Vec3{x + other.x, y + other.y, z + other.z};
        }

        auto operator+(Type const other) const -> Vec3
        {
            return Vec3{x + other, y + other, z + other};
        }

        auto operator/(Type const other) const -> Vec3
        {
            return Vec3{x / other, y / other, z / other};
        }

        auto operator==(Vec3 const& other) const -> bool
        {
            return std::make_tuple(x, y, z) == std::make_tuple(other.x, other.y, other.z);
        }

        auto dot(Vec3 const& other) const -> Type
        {
            return x * other.x + y * other.y + z * other.z;
        }

        auto cross(Vec3 const& other) -> Vec3
        {
            Vec3 vec = *this;
            x = vec.y * other.z - vec.z * other.y;
            y = vec.z * other.x - vec.x * other.z;
            z = vec.x * other.y - vec.y * other.x;
            return *this;
        }
    };

    using Vec3f = Vec3<float>;
    using Vec3d = Vec3<double>;

    template <typename Type>
    struct Vec4
    {
        Type x;
        Type y;
        Type z;
        Type w;

        Vec4() : x(0), y(0), z(0), w(0)
        {
        }

        Vec4(const Type x, const Type y, const Type z, const Type w) : x(x), y(y), z(z), w(w)
        {
        }

        Vec4(Vec4 const& other) : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        Vec4(Vec4&& other) : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        auto operator=(Vec4 const& other) -> Vec4
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }

        auto operator=(Vec4&& other) -> Vec4
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

        auto normalize() -> Vec4&
        {
            Type inverse = static_cast<Type>(1) / this->length();
            x = x * inverse;
            y = y * inverse;
            z = z * inverse;
            w = w * inverse;
            return *this;
        }

        auto length() const -> Type
        {
            return static_cast<Type>(std::sqrt(x * x + y * y + z * z + w * w));
        }

        auto operator*(Type const other) const -> Vec4
        {
            return Vec4{x * other, y * other, z * other, w * other};
        }

        auto operator-(Vec4 const& other) const -> Vec4
        {
            return Vec4{x - other.x, y - other.y, z - other.z, w - other.w};
        }

        auto operator-(Type const other) const -> Vec4
        {
            return Vec4{x - other, y - other, z - other, w - other};
        }

        auto operator-() const -> Vec4
        {
            return Vec4{-x, -y, -z, -w};
        }

        auto operator+(Vec4 const& other) const -> Vec4
        {
            return Vec4{x + other.x, y + other.y, z + other.z, w + other.w};
        }

        auto operator+(Type const other) const -> Vec4
        {
            return Vec4{x + other, y + other, z + other, w + other};
        }

        auto operator/(Type const other) const -> Vec4
        {
            return Vec4{x / other, y / other, z / other, w / other};
        }

        auto operator==(Vec4 const& other) const -> bool
        {
            return std::make_tuple(x, y, z) == std::make_tuple(other.x, other.y, other.z, other.w);
        }
    };

    using Vec4f = Vec4<float>;
    using Vec4d = Vec4<double>;
} // namespace ionengine::math

namespace std
{
    template <typename Type>
    struct hash<ionengine::math::Vec2<Type>>
    {
        auto operator()(ionengine::math::Vec2<Type> const& other) const -> size_t
        {
            return std::hash<Type>()(other.x) ^ std::hash<Type>()(other.y);
        }
    };

    template <typename Type>
    struct hash<ionengine::math::Vec3<Type>>
    {
        auto operator()(ionengine::math::Vec3<Type> const& other) const -> size_t
        {
            return std::hash<Type>()(other.x) ^ std::hash<Type>()(other.y) ^ std::hash<Type>()(other.z);
        }
    };

    template <typename Type>
    struct hash<ionengine::math::Vec4<Type>>
    {
        auto operator()(ionengine::math::Vec4<Type> const& other) const -> size_t
        {
            return std::hash<Type>()(other.x) ^ std::hash<Type>()(other.y) ^ std::hash<Type>()(other.z) ^
                   std::hash<Type>()(other.w);
        }
    };
} // namespace std