#pragma once
#ifndef DRAKO_FILE_FORMATS_JSON_HPP_
#define DRAKO_FILE_FORMATS_JSON_HPP_

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"

#include <string_view>
#include <tuple>

namespace drako::file_formats::json
{
    struct parser_error
    {
    };

    enum class value_type : std::uint8_t
    {
        invalid = 0,
        object,
        array,
        string,
        number
    };

    struct value_array
    {
        size_t         size;
        dom_tree_node* values;
    };

    struct value_object
    {
        size_t            size;
        std::string_view* names;
        dom_tree_node*    values;
    };

    struct value_string
    {
        std::u16string_view value;
    };

    struct value_number
    {
        std::int64_t value_integer;
        double       value_floating;
    };

    class dom_tree_node
    {
    public:
        explicit constexpr dom_tree_node(value_array value) noexcept
            : _payload_type{ value_type::array }
            , _payload_array{ value }
        {
        }

        explicit constexpr dom_tree_node(value_object value) noexcept
            : _payload_type{ value_type::object }
            , _payload_object{ value }
        {
        }

        DRAKO_NODISCARD
        dom_tree_node operator[](size_t index) const noexcept
        {
            DRAKO_ASSERT(_payload_type == value_type::array,
                "This isn't a JSON array");
            return _payload_array.values[index];
        }

        DRAKO_NODISCARD
        dom_tree_node operator[](std::string_view name) const noexcept
        {
            DRAKO_ASSERT(_payload_type == value_type::object,
                "This isn't a JSON object");
        }

        DRAKO_NODISCARD
        bool is_array() const noexcept
        {
            return _payload_type == value_type::array;
        }

        DRAKO_NODISCARD
        bool is_object() const noexcept
        {
            return _payload_type == value_type::object;
        }


        DRAKO_NODISCARD
        int to_int() noexcept;

        DRAKO_NODISCARD
        float to_float() noexcept;

        DRAKO_NODISCARD
        std::tuple<bool, double> to_double() noexcept
        {
            if (_payload_type == value_type::invalid)
                return { false, 0 };

            if (_payload_type != value_type::number)
                return { false, 0 };

            return { true, static_cast<double>(_payload_number.value) };
        }

        friend std::ostream& operator<<(std::ostream&, const dom_tree_node&);

    private:
        value_type _payload_type;
        union
        {
            value_array  _payload_array;
            value_object _payload_object;
            value_string _payload_string;
            value_number _payload_number;
        };
    };


    class dom_tree
    {
    public:
        friend std::ostream& operator<<(std::ostream&, const dom_tree&);

    private:
        dom_tree_node _root;
    };

    class dom_value
    {
    public:
    private:
    };


    struct parser_config
    {
    };

    DRAKO_NODISCARD
    std::tuple<dom_tree, parser_error>
    parse_dom(std::string_view json, const parser_config& config) noexcept;

    DRAKO_NODISCARD
    std::tuple<dom_tree, parser_error>
    parse_dom(std::u16string_view json, const parser_config& config) noexcept;


} // namespace drako::file_formats::json

#endif // !DRAKO_FILE_FORMATS_JSON_HPP_