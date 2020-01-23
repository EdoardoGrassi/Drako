#pragma once
#ifndef DRAKO_WAVEFRONT_PARSER_HPP
#define DRAKO_WAVEFRONT_PARSER_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"

#include <array>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace drako::plugins::wavefront
{
    struct obj_vertex
    {
        float x;
        float y;
        float z;
        float w;
    };

    struct obj_vertex_normal
    {
        float x;
        float y;
        float z;
    };

    struct obj_vertex_texcoord
    {
        float u;
        float v;
        float w;
    };

    class obj_parser_result
    {
    public:
        explicit obj_parser_result() noexcept;

        // Number of vertex in the object
        DRAKO_NODISCARD
        size_t vertex_count() const noexcept
        {
            return std::size(_vertex_points) / 4;
        }

        DRAKO_NODISCARD
        std::array<float, 4> vertex(size_t idx) const noexcept
        {
            idx *= 4;
            return { _vertex_points[idx],
                _vertex_points[idx + 1],
                _vertex_points[idx + 2],
                _vertex_points[idx + 3] };
        }

        DRAKO_NODISCARD
        std::array<float, 3> normal(size_t idx) const noexcept
        {
            idx *= 3;
            return { _vertex_normals[idx], _vertex_normals[idx + 1], _vertex_normals[idx + 2] };
        }

        DRAKO_NODISCARD
        std::array<float, 3> texcoord(size_t idx) const noexcept
        {
            return { _vertex_texcoords_u[idx], _vertex_texcoords_v[idx], _vertex_texcoords_w[idx] };
        }

    private:
        std::string        _obj_name{};
        std::vector<float> _vertex_points;
        std::vector<float> _vertex_normals;
        std::vector<float> _vertex_texcoords_u;
        std::vector<float> _vertex_texcoords_v;
        std::vector<float> _vertex_texcoords_w;
    };

    struct obj_object
    {
        std::string           name;
        std::vector<float[4]> vertex_points;
        std::vector<float[3]> vertex_normals;
        std::vector<float[3]> vertex_texcoords;
    };

    /// @brief Configuration parameters for the parser.
    ///
    struct obj_parser_config
    {
        /// @brief Expected number of objects.
        ///
        size_t expected_object_count;

        /// @brief Expected number of vertices.
        ///
        size_t expected_vertex_count;

        /// @brief Expected number of triangle.
        ///
        size_t expected_triangle_count;
    };

    enum class obj_warning_code
    {
    };

    DRAKO_NODISCARD DRAKO_FORCE_INLINE std::string to_string(obj_warning_code wc) noexcept
    {
        using _warc = obj_warning_code;
        switch (wc)
        {
            default: return "Unknow warning code";
        }
    }

    enum class obj_error_code
    {
        unknown_tag,
        invalid_arg_format,
        tag_f_invalid_args_count,
        tag_f_invalid_args_format,
        tag_o_invalid_args_count,
        tag_v_invalid_args_count,
        tag_vn_invalid_args_count,
        tag_vt_invalid_args_count
    };

    DRAKO_NODISCARD DRAKO_FORCE_INLINE std::string to_string(obj_error_code ec) noexcept
    {
        using _errc = obj_error_code;
        switch (ec)
        {
            case _errc::tag_o_invalid_args_count: return "Tag 'o' requires [1] argument";
            case _errc::tag_v_invalid_args_count: return "Tag 'v' requires [3,4] argument";
            case _errc::tag_vn_invalid_args_count: return "Tag 'vn' requires [3] arguments";
            case _errc::tag_vt_invalid_args_count: return "Tag 'vt' requires [2,3] arguments";
            case _errc::tag_f_invalid_args_count: return "";
            case _errc::tag_f_invalid_args_format: return "";
            default: return "Unknown error code";
        }
    }

    /// @brief Report data of a parsing warning.
    ///
    struct obj_parser_warning
    {
        size_t           line_idx; // index of the line that produced the warning
        size_t           char_idx; // index of the token that produced the warning
        obj_warning_code warc;

        constexpr explicit obj_parser_warning(size_t line, size_t token, obj_warning_code warc)
            : line_idx{ line }
            , char_idx{ token }
            , warc{ warc }
        {
        }
    };

    /// @brief Report data of a parsing error.
    ///
    struct obj_parser_error
    {
        /// @brief Index of the character in the buffer.
        ///
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t
        char_index() const noexcept { return _char_index; }

        /// @brief Line number location of the error.
        ///
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t
        line() const noexcept { return _row_number; }

        /// @brief Column number location of the error.
        ///
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t
        column() const noexcept { return _col_number; }

        /// @brief Error code.
        ///
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::underlying_type_t<obj_error_code>
        value() const noexcept { return static_cast<std::underlying_type_t<obj_error_code>>(_errc); }

        /// @brief Short explanatory description of the error.
        ///
        DRAKO_NODISCARD DRAKO_FORCE_INLINE std::string
        message() const noexcept { return to_string(_errc); }

        size_t         _char_index; // [ 0, size(source) )  , zero-based
        size_t         _row_number; // [ 1, #rows ]         , one-based
        size_t         _col_number; // [ 1, size(col) ]     , one-based
        obj_error_code _errc;
    };

    /// @brief Parse an ASCII string acoording to OBJ format.
    ///
    DRAKO_NODISCARD
    std::tuple<obj_object, std::vector<obj_parser_warning>, std::vector<obj_parser_error>>
    parse(const std::string_view source, const obj_parser_config& config);

} // namespace drako::plugins::wavefront

#endif // !DRAKO_WAVEFRONT_PARSER_HPP