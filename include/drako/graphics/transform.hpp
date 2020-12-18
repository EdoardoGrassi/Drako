#pragma once
#ifndef DRAKO_TRANSFORM_HPP
#define DRAKO_TRANSFORM_HPP

#include "drako/graphics/camera_types.hpp"
#include "drako/math/mat4x4.hpp"
#include "drako/math/quaternion.hpp"
#include "drako/math/vector3.hpp"

namespace drako
{
    // Creates a translation transform matrix.
    [[nodiscard]] inline constexpr Mat4x4 translate(float _x_, float _y_, float _z_) noexcept
    {
        /* clang-format off */
        return Mat4x4{ { 1.f, 0.f, 0.f, _x_,
                         0.f, 1.f, 0.f, _y_,
                         0.f, 0.f, 1.f, _z_,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }

    [[nodiscard]] inline constexpr Mat4x4 translate(Vec3 v) noexcept
    {
        return translate(v[0], v[1], v[2]);
    }

    // Creates a rotation transform matrix.
    [[nodiscard]] inline constexpr Mat4x4 rotate(float x, float y, float z) noexcept;
    [[nodiscard]] inline constexpr Mat4x4 rotate(Quat r) noexcept
    {
        // TODO: implementation
        return Mat4x4::identity();
    }

    [[nodiscard]] inline Mat4x4 rotate_x(float radians) noexcept
    {
        const auto cos = std::cos(radians);
        const auto sin = std::sin(radians);
        /* clang-format off */
        return Mat4x4{ { 1.f, 0.f, 0.f, 0.f,
                         0.f, cos, -sin, 0.f,
                         0.f, sin, cos, 0.f,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }

    [[nodiscard]] inline Mat4x4 rotate_y(float radians) noexcept
    {
        const auto cos = std::cos(radians);
        const auto sin = std::sin(radians);
        /* clang-format off */
        return Mat4x4{ { cos, 0.f, sin, 0.f,
                         0.f, 1.f, 0.f, 0.f,
                        -sin, 0.f, cos, 0.f,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }

    [[nodiscard]] inline Mat4x4 rotate_z(float radians) noexcept
    {
        const auto cos = std::cos(radians);
        const auto sin = std::sin(radians);
        /* clang-format off */
        return Mat4x4{ { cos, -sin, 0.f, 0.f,
                         sin, cos, 0.f, 0.f,
                         0.f, 0.f, 1.f, 0.f,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }



    [[nodiscard]] inline constexpr Mat4x4 scale(float _x_, float _y_, float _z_) noexcept
    {
        /* clang-format off */
        return Mat4x4{ { _x_, 0.f, 0.f, 0.f,
                         0.f, _y_, 0.f, 0.f,
                         0.f, 0.f, _z_, 0.f,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }

    // Creates a scaling transform matrix.
    [[nodiscard]] inline constexpr Mat4x4 scale(float s) noexcept
    {
        return scale(s, s, s);
    }

    [[nodiscard]] inline constexpr Mat4x4 scale(const Vec3& s) noexcept
    {
        return scale(s[0], s[1], s[2]);
    }

    [[nodiscard]] inline constexpr Mat4x4 shearing_xy(float _x_, float _y_) noexcept
    {
        /* clang-format off */
        return Mat4x4{ { 1.f, 0.f, _x_, 0.f,
                         0.f, 1.f, _y_, 0.f,
                         0.f, 0.f, 1.f, 0.f,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }

    [[nodiscard]] inline constexpr Mat4x4 shearing_yz(float _y_, float _z_) noexcept
    {
        /* clang-format off */
        return Mat4x4{ { 1.f, 0.f, 0.f, 0.f,
                         _y_, 1.f, 0.f, 0.f,
                         _z_, 0.f, 1.f, 0.f,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }

    [[nodiscard]] inline constexpr Mat4x4 shearing_xz(float _x_, float _z_) noexcept
    {
        /* clang-format off */
        return Mat4x4{ { 1.f, _x_, 0.f, 0.f,
                         0.f, 1.f, 0.f, 0.f,
                         0.f, _z_, 1.f, 0.f,
                         0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */
    }

    // Creates a translation, rotation and scaling transform matrix.
    [[nodiscard]] inline constexpr Mat4x4 transform(Vec3 p, Quat r, Vec3 s) noexcept
    {
        return translate(p) * rotate(r) * scale(s);
    }

    // Creates an ortographic projection matrix.
    [[nodiscard]] inline constexpr Mat4x4
    ortographic(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) noexcept
    {
        const float sxx = 2.f / (x_max - x_min); // normalize x in range [-1, 1]
        const float syy = 2.f / (y_max - y_min); // normalize y in range [-1, 1]
        const float szz = 1.f / (z_max - z_min); // normalize z in range [0, 1]

        /* clang-format off */
        Mat4x4 scale{ { sxx, 0.f, 0.f, 0.f,
                        0.f, syy, 0.f, 0.f,
                        0.f, 0.f, szz, 0.f,
                        0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */

        // translate to origin (0,0,0)
        const float txx = -(x_max + x_min) / 2.f;
        const float tyy = -(y_max + y_min) / 2.f;
        const float tzz = -(z_max + z_min) / 2.f;

        /* clang-format off */
        Mat4x4 translate{ { 1.f, 0.f, 0.f, txx,
                            0.f, 1.f, 0.f, tyy,
                            0.f, 0.f, 1.f, tzz,
                            0.f, 0.f, 0.f, 1.f } };
        /* clang-format on */

        return scale * translate;
    }

    // Creates a perspective projection matrix.
    [[nodiscard]] inline constexpr Mat4x4
    perspective(float l, float r, float b, float t, float n, float f) noexcept
    {
        const float sxx = 2 * n / (r - l);
        const float syy = 2 * n / (t - b);
        const float szz = f / (f - n);
        const float sww = -f * n / (f - n);

        const float txx = -(r + l) / (r - l);
        const float tyy = -(t + b) / (t - b);

        /* clang-format off */
        return Mat4x4{ { sxx, 0.f, txx, 0.f,
                         0.f, syy, tyy, 0.f,
                         0.f, 0.f, szz, sww,
                         0.f, 0.f, 1.f, 1.f } };
        /* clang-format on */
    }

    // Rotates about axis passing through point in world space by angle degrees.
    [[nodiscard]] constexpr Mat4x4 rotate_around(Vec3 point, Vec3 axis, float angle) noexcept;

    [[nodiscard]] constexpr Mat4x4 look_at(Vec3 camera, Vec3 target, norm3 up) noexcept;

} // namespace drako

#endif // !DRAKO_TRANSFORM_HPP