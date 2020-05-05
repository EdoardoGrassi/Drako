#pragma once
#ifndef DRAKO_GRAPHICS_CAMERA_HPP
#define DRAKO_GRAPHICS_CAMERA_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/mat4x4.hpp"

namespace drako::gpx
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

    private:

        mat4x4 _proj; // Projection matrix.
    };
} // namespace drako::gpx

#endif // !DRAKO_GRAPHICS_CAMERA_HPP