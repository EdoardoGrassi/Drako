#pragma once
#ifndef DRAKO_FILE_FORMATS_WAVEFRONT_PARSER_HPP
#define DRAKO_FILE_FORMATS_WAVEFRONT_PARSER_HPP

#include "drako/file_formats/wavefront/object.hpp"

#include <array>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace drako::file_formats::obj
{
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
            case _pec::tag_o_invalid_args_count: return "Tag 'o' requires one argument";
            case _pec::tag_v_invalid_args_count: return "Tag 'v' requires 3 or 4 arguments";
            case _pec::tag_vn_invalid_args_count: return "Tag 'vn' requires 3 arguments";
            case _pec::tag_vt_invalid_args_count: return "Tag 'vt' requires 2 or 3 arguments";
            case _pec::tag_f_invalid_args_count: return "";
            case _pec::tag_f_invalid_args_format: return "";
            default: return "Unknown error code";
        }
    }


    class parser_error : public std::runtime_error
    {
    public:
        explicit parser_error(parser_error_code ec)
            : std::runtime_error(to_string(ec)), _ec{ ec }
        {
        }

    private:
        parser_error_code _ec;
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


    /// @brief Result of parsing of an .obj file.
    struct parser_result
    {
        std::vector<object> objects;
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

    [[nodiscard]] parser_result parse(const std::string_view source, const parser_config& config);

    /// @brief Parse an ASCII string according to OBJ format.
    ///
    /// Stops at first parsing error.
    ///
    [[nodiscard]] std::variant<parser_result, parser_error_report>
    try_parse(const std::string_view source, const parser_config& config) noexcept;

    /// @brief Parse an ASCII string acoording to OBJ format.
    ///
    [[nodiscard]] std::tuple<parser_result, std::vector<parser_error_report>>
    try_force_parse(const std::string_view source, const parser_config& config) noexcept;

} // namespace drako::file_formats::obj

#endif // !DRAKO_FILE_FORMATS_WAVEFRONT_PARSER_HPP