#pragma once
#ifndef DRAKO_INPUT_DEVICE_HPP_
#define DRAKO_INPUT_DEVICE_HPP_

#include <cstdint>
#include <string>

namespace drako::engine
{
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

    // Describes the capabilities of an input device
    struct input_device_layout
    {
        std::vector<input_axis_info>   input_axis;
        std::vector<input_button_info> input_buttons;
    };

} // namespace drako::engine

#endif // !DRAKO_INPUT_DEVICE_HPP_