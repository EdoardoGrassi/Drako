#pragma once
#ifndef DRAKO_JSON_HPP
#define DRAKO_JSON_HPP

#include <cassert>
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

    class dom_tree_node;

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

        [[nodiscard]] dom_tree_node operator[](size_t index) const noexcept
        {
            assert(_payload_type == value_type::array); // "This is not a JSON array");
            return _payload_array.values[index];
        }

        [[nodiscard]] dom_tree_node operator[](std::string_view name) const noexcept
        {
            assert(_payload_type == value_type::object); // "This is not a JSON object");
            //return _payload_object;
        }

        [[nodiscard]] bool is_array() const noexcept
        {
            return _payload_type == value_type::array;
        }

        [[nodiscard]] bool is_object() const noexcept
        {
            return _payload_type == value_type::object;
        }


        [[nodiscard]] int to_int() noexcept;

        [[nodiscard]] float to_float() noexcept;

        [[nodiscard]] std::tuple<bool, double> to_double() noexcept
        {
            if (_payload_type == value_type::invalid)
                return { false, 0 };

            if (_payload_type != value_type::number)
                return { false, 0 };

            return { true, static_cast<double>(_payload_number.value_floating) };
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

    [[nodiscard]] std::tuple<dom_tree, parser_error>
    parse_dom(std::string_view json, const parser_config& config) noexcept;

    [[nodiscard]] std::tuple<dom_tree, parser_error>
    parse_dom(std::u16string_view json, const parser_config& config) noexcept;


} // namespace drako::file_formats::json

#endif // !DRAKO_JSON_HPP