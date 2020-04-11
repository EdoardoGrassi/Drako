#pragma once
#ifndef DRAKO_FILE_FORMATS_WAVEFRONT_OBJ_HPP_
#define DRAKO_FILE_FORMATS_WAVEFRONT_OBJ_HPP_

#include <array>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
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

    class parser_result
    {
    public:
        explicit parser_result() noexcept;

        // Number of vertex in the object
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
        std::string           _obj_name{};
        std::vector<float>    _vertex_points;
        std::vector<float>    _vertex_normals;
        std::vector<float>    _vertex_texcoords_u;
        std::vector<float>    _vertex_texcoords_v;
        std::vector<float>    _vertex_texcoords_w;
        std::vector<uint32_t> _faces;
    };

    struct object
    {
        std::string        name;
        std::vector<float> vertex_points;
        std::vector<float> vertex_normals;
        std::vector<float> vertex_texcoords;
        std::vector<float> faces;

        /*
        DRAKO_NODISCARD std::string_view name() const noexcept
        {
            return _name;
        }
        */
    };

    /// @brief Configuration parameters for the parser.
    ///
    struct parser_config
    {
        /// @brief Expected number of objects.
        ///
        size_t expected_object_count = 1;

        /// @brief Expected number of vertices.
        ///
        size_t expected_vertex_count = 10'000;

        /// @brief Expected number of triangle.
        ///
        size_t expected_triangle_count = 10'000;
    };

    enum class parser_warning_code
    {
    };

    [[nodiscard]] std::string to_string(parser_warning_code wc) noexcept
    {
        using _pwc = parser_warning_code;
        switch (wc)
        {
            default: return "Unknow warning code";
        }
    }

    enum class parser_error_code
    {
        unknown_tag,
        index_out_of_range,
        invalid_arg_format,
        tag_f_invalid_args_count,
        tag_f_invalid_args_format,
        tag_o_invalid_args_count,
        tag_v_invalid_args_count,
        tag_vn_invalid_args_count,
        tag_vt_invalid_args_count
    };

    [[nodiscard]] std::string to_string(parser_error_code ec) noexcept
    {
        using _pec = parser_error_code;
        switch (ec)
        {
            case _pec::index_out_of_range: return "Index out of valid range";
            case _pec::tag_o_invalid_args_count: return "Tag 'o' requires [1] argument";
            case _pec::tag_v_invalid_args_count: return "Tag 'v' requires [3,4] argument";
            case _pec::tag_vn_invalid_args_count: return "Tag 'vn' requires [3] arguments";
            case _pec::tag_vt_invalid_args_count: return "Tag 'vt' requires [2,3] arguments";
            case _pec::tag_f_invalid_args_count: return "";
            case _pec::tag_f_invalid_args_format: return "";
            default: return "Unknown error code";
        }
    }

    /// @brief Report data of a parsing warning.
    ///
    struct parser_warning_report
    {
        size_t              row; // index of the line that produced the warning
        size_t              col; // index of the token that produced the warning
        size_t              offset;
        parser_warning_code warc;

        constexpr explicit parser_warning_report(
            size_t row, size_t col, size_t offset, parser_warning_code wc) noexcept
            : row{ row }
            , col{ col }
            , offset{ offset }
            , warc{ wc }
        {
        }
    };

    /// @brief Report data of a parsing error.
    ///
    class parser_error_report
    {
    public:
        explicit constexpr parser_error_report(
            size_t file_row_number, size_t file_column_number,
            size_t file_char_offset, parser_error_code ec) noexcept
            : _char_index{ file_char_offset }
            , _row_number{ file_row_number }
            , _col_number{ file_column_number }
            , _errc{ ec }
        {
        }

        /// @brief Index of the character in the buffer.
        ///
        [[nodiscard]] constexpr size_t
        char_index() const noexcept { return _char_index; }

        /// @brief Line number location of the error.
        ///
        [[nodiscard]] constexpr size_t
        line() const noexcept { return _row_number; }

        /// @brief Column number location of the error.
        ///
        [[nodiscard]] constexpr size_t
        column() const noexcept { return _col_number; }

        /// @brief Error code.
        ///
        [[nodiscard]] constexpr std::underlying_type_t<parser_error_code>
        value() const noexcept { return static_cast<std::underlying_type_t<parser_error_code>>(_errc); }

        /// @brief Short explanatory description of the error.
        ///
        [[nodiscard]] std::string
        message() const noexcept { return to_string(_errc); }

    private:
        size_t            _char_index; // [ 0, size(source) )  , zero-based
        size_t            _row_number; // [ 1, #rows ]         , one-based
        size_t            _col_number; // [ 1, size(col) ]     , one-based
        parser_error_code _errc;
    };

    struct parser_result_report
    {
        std::vector<object> objects;
    };


    /// @brief Parse an ASCII string according to OBJ format.
    ///
    /// Stops at first parsing error.
    ///
    [[nodiscard]] std::variant<parser_result_report, parser_error_report>
    parse(const std::string_view source, const parser_config& config);

    /// @brief Parse an ASCII string acoording to OBJ format.
    ///
    [[nodiscard]] std::tuple<parser_result_report, std::vector<parser_error_report>>
    force_parse(const std::string_view source, const parser_config& config);

} // namespace drako::file_formats::obj

#endif // !DRAKO_FILE_FORMATS_WAVEFRONT_OBJ_HPP_