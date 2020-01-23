#pragma once
#ifndef DRAKO_SPHERE_HPP
#define DRAKO_SPHERE_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/line.hpp"
#include "drako/math/vector3.hpp"

namespace drako
{
    // TODO change vector_3f to avoid the hidden extra byte

    // Represents a sphere in 3D space.
    //
    struct alignas(sizeof(float) * 4) sphere
    {
        vec3  center; // sphere center.
        float radius; // sphere radius.
    };

    // Test if a point P is contained in sphere S volume.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool
    overlap(const vec3& p, const sphere& s) noexcept
    {
        const auto distance = p - s.center;
        return dot(distance, distance) <= (s.radius * s.radius);
    }

    // Overlap test between spheres.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool
    test_overlap(const sphere& a, const sphere& b) noexcept
    {
        // Distance between centers
        const vec3 d = a.center - b.center;
        // Sum of radii
        const float r = a.radius + b.radius;
        // Avoid square root
        return dot(d, d) <= (r * r);
    }

    // Computes intersection between ray R and sphere S.
    // Returns t value of the first intersection and intersection point Q.
    DRAKO_NODISCARD constexpr bool
    intersect(const line3d r, const sphere& s, float& t, vec3& q) noexcept
    {
        auto const m = r.origin - s.center;
        auto const b = dot(m, r.normal);
        auto const c = dot(m, m) - s.radius * s.radius;

        // Exit if ray origin is outside sphere and is pointing away from it
        if (c > 0.0f && b > 0.0f)
        {
            return false;
        }

        auto const discr = b * b - c;
        // Negative discriminant, no intersections
        if (discr < 0.0f)
        {
            return false;
        }

        // Compute smallest t of intersection
        t = (-b) - sqrt(discr);

        // If t is negative, ray started inside sphere so clamp to zero.
        t = clamp_to_0(t);

        q = r.origin + t * d;
        return true;
    }

} // namespace drako

#endif // !DRAKO_SPHERE_HPP