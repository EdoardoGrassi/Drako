#pragma once
#ifndef DRAKO_FILE_FORMATS_WAVEFRONT_PARSER_HPP
#define DRAKO_FILE_FORMATS_WAVEFRONT_PARSER_HPP

#include "drako/file_formats/wavefront/object.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace drako::file_formats::obj
{
    enum class parser_error_code
    {
        duplicate_object_name,
        index_out_of_range,
        invalid_arg_count,
        invalid_arg_format,
        tag_f_invalid_args_count,
        tag_f_invalid_args_format,
        tag_o_invalid_args_count,
        tag_v_invalid_args_count,
        tag_vn_invalid_args_count,
        tag_vt_invalid_args_count,
        unknown_tag
    };

    [[nodiscard]] inline std::string to_string(parser_error_code ec) noexcept
    {
        using _pec = parser_error_code;
        switch (ec)
        {
            case _pec::duplicate_object_name: return "Multiple object names.";
            case _pec::invalid_arg_count: return "Invalid arguments count.";
            case _pec::invalid_arg_format: return "Invalid argument format.";
            case _pec::index_out_of_range: return "Index out of valid range";
            case _pec::tag_o_invalid_args_count: return "Tag 'o' requires one argument.";
            case _pec::tag_v_invalid_args_count: return "Tag 'v' requires 3 or 4 arguments.";
            case _pec::tag_vn_invalid_args_count: return "Tag 'vn' requires 3 arguments.";
            case _pec::tag_vt_invalid_args_count: return "Tag 'vt' requires 2 or 3 arguments.";
            case _pec::tag_f_invalid_args_count: return "";
            case _pec::tag_f_invalid_args_format: return "";
            case _pec::unknown_tag: return "Unknown tag.";
            default: return "Unknown error code.";
        }
    }


    class parser_error : public std::runtime_error
    {
    public:
        explicit parser_error(parser_error_code ec)
            : std::runtime_error{ to_string(ec) }, _ec{ ec } {}

    private:
        parser_error_code _ec;
    };

    class parser_token_error : public parser_error
    {
    public:
        explicit parser_token_error(parser_error_code ec)
            : parser_error{ ec } {}

    private:
        std::string_view _token;
    };


    /// @brief Configuration parameters for the parser.
    ///
    struct parser_config
    {
        /// @brief Enable specific extensions to .obj standard.
        enum class extension
        {
            standard     = 0,
            vertex_color = (1 << 0),
        };

        /// @brief Expected number of objects.
        ///
        size_t expected_object_count = 1;

        /// @brief Expected number of vertices.
        ///
        size_t expected_vertex_count = 10'000;

        /// @brief Expected number of triangle.
        ///
        size_t expected_triangle_count = 10'000;

        extension flags = extension::standard;
    };


    /// @brief Result of parsing of an .obj file.
    struct parser_result
    {
        mesh_data           data;
        std::vector<object> objects;
        std::vector<group>  groups;
    };

    /*
    enum class parser_warning_code
    {
    };

    [[nodiscard]] inline std::string to_string(parser_warning_code wc) noexcept
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
            : row{ row }, col{ col }, offset{ offset }, warc{ wc } {}
    };*/

    /// @brief Report data of a parsing error.
    ///
    class parser_error_report
    {
    public:
        explicit constexpr parser_error_report(
            size_t row, size_t column, size_t offset, parser_error_code ec) noexcept
            : _char_index{ offset }, _row{ row }, _col{ column }, _ec{ ec } {}

        /// @brief Index of the character in the buffer.
        ///
        [[nodiscard]] constexpr size_t
        char_index() const noexcept { return _char_index; }

        /// @brief Line number location of the error.
        ///
        [[nodiscard]] constexpr size_t
        line() const noexcept { return _row; }

        /// @brief Column number location of the error.
        ///
        [[nodiscard]] constexpr size_t
        column() const noexcept { return _col; }

        /// @brief Error code.
        ///
        [[nodiscard]] constexpr std::underlying_type_t<parser_error_code>
        value() const noexcept { return static_cast<std::underlying_type_t<parser_error_code>>(_ec); }

        /// @brief Short explanatory description of the error.
        ///
        [[nodiscard]] std::string
        message() const noexcept { return to_string(_ec); }

    private:
        size_t            _char_index; // [ 0, size(source) )  , zero-based
        size_t            _row;        // [ 1, #rows ]         , one-based
        size_t            _col;        // [ 1, size(col) ]     , one-based
        parser_error_code _ec;
    };


    class parser
    {
    public:
        explicit parser(const parser_config& c, const std::size_t buffer_size)
            : _config{ c }, _size{ buffer_size }, _buffer{ std::make_unique<std::byte[]>(_size) } {}

        [[nodiscard]] parser_result parse_file();
        [[nodiscard]] parser_result parse_string(const std::string_view s);

    private:
        parser_config                _config;
        std::size_t                  _size;
        std::unique_ptr<std::byte[]> _buffer;
    };


#if !defined(_drako_disable_exceptions) /*vvv implementation with exceptions vvv*/

    [[nodiscard]] parser_result parse(const std::string_view source, const parser_config& config);

#else /*^^^ implementation with exceptions ^^^/vvv implementation with error codes vvv*/


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

#endif /*^^^ implementation with error codes ^^^*/

} // namespace drako::file_formats::obj

#endif // !DRAKO_FILE_FORMATS_WAVEFRONT_PARSER_HPP