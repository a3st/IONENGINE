// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/vector.hpp"

namespace ionengine::core
{
    template <typename Type>
    struct Plane
    {
        core::Vec3<Type> n;
        Type d;

        Plane() : d(0)
        {
        }

        Plane(core::Vec3<Type> const& n, Type const& d) : n(n), d(d)
        {
        }

        Plane(Vec3<Type> const& a, Vec3<Type> const& b, Vec3<Type> const& c)
        {
            Vec3<Type> const u = b - a;
            Vec3<Type> const v = c - a;
            n = u.cross(v);
            d = -a.dot(n);
        }

        auto data() const -> Type const*
        {
            return &n;
        }

        auto size() const -> size_t
        {
            return 4;
        }

        auto side(Vec3<Type> const& other) const -> bool
        {
            return false;
        }

        auto translate(Vec3<Type> const& position) const -> Plane
        {
            return Plane{n.x, n.y, n.z, d - position.dot(n)};
        }
    };

    using Planef = Plane<float>;
    using Planed = Plane<double>;
} // namespace ionengine::core