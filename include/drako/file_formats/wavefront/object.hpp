#pragma once
#ifndef DRAKO_FORMATS_WAVEFRONT_OBJECT_HPP
#define DRAKO_FORMATS_WAVEFRONT_OBJECT_HPP

#include <array>
#include <string>
#include <vector>

namespace drako::file_formats::obj
{
    struct vertex
    {
        float x;
        float y;
        float z;
        float w;
    };

    struct vertex_normal
    {
        float x;
        float y;
        float z;
    };

    struct vertex_texcoord
    {
        float u;
        float v;
        float w;
    };

    /// @brief A single mesh object.
    class object
    {
    public:
        explicit object();

        object(const object&) = delete;
        object& operator=(const object&) = delete;

        [[nodiscard]] size_t vertex_count() const noexcept
        {
            return std::size(_vertex_points) / 4;
        }

        [[nodiscard]] std::array<float, 4> vertex(size_t i) const noexcept
        {
            return { _vertex_points[i * 4],
                _vertex_points[i * 4 + 1],
                _vertex_points[i * 4 + 2],
                _vertex_points[i * 4 + 3] };
        }

        [[nodiscard]] std::array<float, 3> normal(size_t i) const noexcept
        {
            return { _vertex_normals[i * 3], _vertex_normals[i * 3 + 1], _vertex_normals[i * 3 + 2] };
        }

        [[nodiscard]] std::array<float, 3> texcoord(size_t i) const noexcept
        {
            return { _vertex_texcoords_u[i], _vertex_texcoords_v[i], _vertex_texcoords_w[i] };
        }

        [[nodiscard]] std::array<uint32_t, 3> triangle(size_t i) const noexcept
        {
            return { _faces[i * 3], _faces[i * 3 + 1], _faces[i * 3 + 2] };
        }

    private:
        std::string           _name;
        std::vector<float>    _vertex_points;
        std::vector<float>    _vertex_normals;
        std::vector<float>    _vertex_texcoords_u;
        std::vector<float>    _vertex_texcoords_v;
        std::vector<float>    _vertex_texcoords_w;
        std::vector<uint32_t> _faces;
    };
} // namespace drako::file_formats::obj

#endif // !DRAKO_FORMATS_WAVEFRONT_OBJECT_HPP