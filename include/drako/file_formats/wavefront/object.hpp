#pragma once
#ifndef DRAKO_FORMATS_WAVEFRONT_OBJECT_HPP
#define DRAKO_FORMATS_WAVEFRONT_OBJECT_HPP

#include <array>
#include <string>
#include <vector>

namespace drako::file_formats::obj
{
    using value = float;
    using index = std::int32_t;

    /// @brief Geometric vertex.
    struct alignas(16) vertex
    {
        explicit constexpr vertex(value x, value y, value z, value w) noexcept
            : x{ x }, y{ y }, z{ z }, w{ w } {}

        /// @brief X component.
        value x;

        /// @brief The y component.
        value y;

        /// @brief The z component.
        value z;

        /// @brief Vertex weight of the vector.
        value w = 0.f;
    };

    struct alignas(16) normal
    {
        explicit constexpr normal(value x, value y, value z) noexcept
            : x{ x }, y{ y }, z{ z } {}

        value x;
        value y;
        value z;
    };

    struct alignas(16) texcoord
    {
        explicit constexpr texcoord(value u, value v, value w) noexcept
            : u{ u }, v{ v }, w{ w } {}

        value u;
        value v = 0.f;
        value w = 0.f;
    };


    struct triplette
    {
        index v, vt, vn;
    };


    struct face
    {
        //std::array<index, 3> v;
        //std::array<index, 3> vn;
        //std::array<index, 3> vt;
        std::array<triplette, 3> triplets;
    };

    /// @brief Mesh data from a whole .obj file.
    struct mesh_data
    {
        std::vector<vertex>   v;
        std::vector<normal>   vn;
        std::vector<texcoord> vt;
    };

    /// @brief Group of element under the same group tag.
    struct group
    {
        std::string name;
        //std::vector<index> points;
        //std::vector<index> lines;
        std::vector<index> faces;
    };

    /// @brief Group of elements under the same object tag.
    struct object
    {
        std::string name;
        //std::vector<index> points;
        //std::vector<index> lines;
        std::vector<index> faces;
    };


} // namespace drako::file_formats::obj

#endif // !DRAKO_FORMATS_WAVEFRONT_OBJECT_HPP