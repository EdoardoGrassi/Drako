#pragma once
#ifndef DRAKO_INPUT_SYSTEM_TYPES_HPP
#define DRAKO_INPUT_SYSTEM_TYPES_HPP

#include "drako/core/typed_handle.hpp"

#include <array>
#include <bitset>
#include <cstdint>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

namespace drako::input
{
    DRAKO_DEFINE_TYPED_ID(control_id, std::uint16_t);

    struct input_axis_info
    {
        std::string name;
        float       range_min;
        float       range_max;
        float       deadzone_min;
        float       deadzone_max;
        bool        invert;
    };

    struct input_button_info
    {
        std::string name;
    };

    struct virtual_axis_binding
    {
        std::uint8_t  id;
        std::uint16_t action;
    };

    struct virtual_button_binding
    {
    };


    struct axis_layout_attribute
    {
        std::string name;
    };

    struct button_layout_attribute
    {
        std::string name;
    };

    /*// Describes the capabilities of an input device.
    struct input_device_layout
    {
        std::vector<input_axis_info>   input_axis;
        std::vector<input_button_info> input_buttons;
    };*/

} // namespace drako::input

#endif // !DRAKO_INPUT_SYSTEM_TYPES_HPP