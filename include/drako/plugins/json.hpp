#pragma once
#ifndef DRAKO_JSON_HPP
#define DRAKO_JSON_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"

#include <string_view>
#include <vector>

namespace drako::plugins
{
    enum class json_type : std::uint8_t
    {
        invalid = 0,
        object,
        array,
        string,
        number
    };

    struct json_array
    {
        size_t      size;
        json_value* values;
    };

    struct json_object
    {
        size_t            size;
        std::string_view* names;
        json_value*       values;
    };

    struct json_string
    {
    };

    struct json_number
    {
        double value;
    };

    class json_value
    {
    public:
        explicit constexpr json_value(json_type type) noexcept
            : _payload_type(type)
        {
        }

        DRAKO_NODISCARD
        json_value operator[](size_t index) const noexcept
        {
            DRAKO_ASSERT(_payload_type == json_type::array,
                "This isn't a JSON array");
            return _payload_array.values[index];
        }

        DRAKO_NODISCARD
        json_value operator[](std::string_view name) const noexcept
        {
            DRAKO_ASSERT(_payload_type == json_type::object,
                "This isn't a JSON object");
        }

        DRAKO_NODISCARD
        bool is_array() const noexcept
        {
            return _payload_type == json_type::array;
        }

        DRAKO_NODISCARD
        bool is_object() const noexcept
        {
            return _payload_type == json_type::object;
        }


        DRAKO_NODISCARD
        int to_int() noexcept;

        DRAKO_NODISCARD
        float to_float() noexcept;

        DRAKO_NODISCARD
        std::tuple<bool, double> to_double() noexcept
        {
            if (_payload_type == json_type::invalid)
                return { false, 0 };

            if (_payload_type != json_type::number)
                return { false, 0 };

            return { true, static_cast<double>(_payload_number.value) };
        }

    private:
        json_type _payload_type;
        union
        {
            json_array  _payload_array;
            json_object _payload_object;
            json_string _payload_string;
            json_number _payload_number;
        };
    };

    class json_tree
    {
    public:
    private:
        json_value _root;
    };


    struct json_parser_config
    {
    };

    void parse_json(std::string_view json, const json_parser_config& config) noexcept;


} // namespace drako::plugins

#endif // !DRAKO_JSON_HPP