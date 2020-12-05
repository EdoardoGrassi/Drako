#pragma once
#ifndef DRAKO_PLANE_HPP
#define DRAKO_PLANE_HPP

#include "drako/core/compiler.hpp"
#include "drako/math/utility.hpp"
#include "drako/math/vector3.hpp"

namespace drako
{
    // Represents a plane in 3d space.
    // Stored in constant-normal form.
    struct plane3d
    {
        // Plane normal.
        // Points on the plane satisfy dot(n, x) = d.
        vec3 n;

        // Coefficient of the constant-normal form of the plane.
        // Computed from dot(n, p) = d, for a given point p on the plane.
        float d;

        // Constructs the plane from three non collinear points.
        // Points are considered in counter-clockwise order.
        constexpr explicit plane3d(const vec3 a, const vec3 b, const vec3 c) noexcept
            : n{ normalize(cross(b - a, c - a)) }
            , d{ dot(n, a) }
        {
        }

        // Constructs the plane from a normal and a point on the plane.
        constexpr explicit plane3d(const vec3 p, const norm3 n) noexcept;

        // Constructs the plane from a normal and a distance from the origin.
        constexpr explicit plane3d(const norm3 n, const float d) noexcept;
    };


    // Given a point, computes the closest point on the plane.
    DRAKO_NODISCARD constexpr vec3
    closest_point(const vec3 q, const plane3d p) noexcept
    {
        DRAKO_ASSERT(std::abs(dot(p.n, p.n) - 1.f) < std::numeric_limits<float>::epsilon(),
            "Plane equation must be normalized to allow formula reduction");
        // full formula: (dot(q, p.normal) - p.distance) / dot(p.normal, p.normal)
        // here assuming plane equation normalized, ||p.normal|| == 1
        const float t = dot(p.n, q) - p.d;
        return q - (t * p.n);
    }

    // Computes the signed distance of a point from plane.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float
    distance(vec3 const q, plane3d const p) noexcept
    {
        DRAKO_ASSERT(std::abs(dot(p.n, p.n) - 1.f) < std::numeric_limits<float>::epsilon(),
            "Plane equation must be normalized to allow formula reduction");
        // full formula: (dot(q, p.normal) - p.distance) / dot(p.normal, p.normal)
        // here assuming plane equation normalized, ||p.normal|| == 1
        return dot(q, p.n) - p.d;
    }

    DRAKO_NODISCARD constexpr bool
    intersect(const vec3 a, const vec3 b, const plane3d p) noexcept
    {
        const auto  ab = b - a;
        const float t  = (p.d - dot(p.n, a)) / dot(p.n, ab);
        return (t >= 0.0f) & (t <= 1.0f);
    }

    // TODO: return results as combined struct or tuple
    DRAKO_NODISCARD constexpr bool
    intersect(const vec3 a, const vec3 b, const plane3d p, vec3& out_q) noexcept
    {
        // Compute t value for the directed segment ab intersecationg the plane
        const auto  ab = b - a;
        const float t  = (p.d - dot(p.n, a)) / dot(p.n, ab);

        // If t in [0..1] compute intersection point
        if (t >= 0.0f && t <= 1.0f)
        {
            out_q = a + t * ab;
            return true;
        }
        // No intersection
        return false;
    }

} // namespace drako

#endif // !DRAKO_MATH_PLANE_HPP