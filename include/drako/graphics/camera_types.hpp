#pragma once
#ifndef DRAKO_CAMERA_TYPES_HPP
#define DRAKO_CAMERA_TYPES_HPP

#include "drako/math/mat4x4.hpp"

namespace drako
{
    // Geometrical view frustum.
    struct camera_frustum
    {
        float xmin;
        float xmax;
        float ymin;
        float ymax;

        // Z-depth near plane.
        float zmin;

        // Z-depth far plane.
        float zmax;

        explicit constexpr camera_frustum(
            float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) noexcept;

        explicit constexpr camera_frustum(
            float xfov, float yfov, float zmin, float zmax) noexcept;
    };


    // Virtual model of a pinhole camera that can be use with rasterization-based rendering.
    class render_camera
    {
    public:
        explicit render_camera() noexcept = default;

    private:
        Mat4x4 _proj; // Projection matrix.
    };

    // Creates a perspective projection matrix.
    [[nodiscard]] constexpr Mat4x4 perspective(const camera_frustum& f) noexcept
    {
        const float sxx = 2 * f.zmin / (f.xmax - f.xmin);
        const float syy = 2 * f.zmin / (f.ymax - f.ymin);
        const float szz = f.zmax / (f.zmax - f.zmin);
        const float sww = -f.zmax * f.zmin / (f.zmax - f.zmin);

        const float txx = -(f.xmax + f.xmin) / (f.xmax - f.xmin);
        const float tyy = -(f.ymax + f.ymin) / (f.ymax - f.ymin);

        /* clang-format off */
        return Mat4x4{ { sxx, 0.f, txx, 0.f,
                         0.f, syy, tyy, 0.f,
                         0.f, 0.f, szz, sww,
                         0.f, 0.f, 1.f, 1.f } };
        /* clang-format on */
    }

} // namespace drako

#endif // !DRAKO_CAMERA_TYPES_HPP