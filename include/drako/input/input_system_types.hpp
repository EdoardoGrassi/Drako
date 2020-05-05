#pragma once
#ifndef DRAKO_INPUT_SYSTEM_TYPES_HPP_
#define DRAKO_INPUT_SYSTEM_TYPES_HPP_

#include <array>
#include <bitset>
#include <cstdint>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

namespace drako::engine::input
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


    struct axis_layout_attribute
    {
        std::string name;
    };

    struct button_layout_attribute
    {
        std::string name;
    };

    // Describes the capabilities of an input device.
    struct input_device_layout
    {
        std::vector<input_axis_info>   input_axis;
        std::vector<input_button_info> input_buttons;
    };


    // Describes the current state of an input device.
    struct input_device_state
    {
        std::bitset<64>       buttons;
        std::array<float, 10> axes;
    };


    // Describes an event occured on an input device.
    struct input_device_event
    {
    };


    class input_device_interface
    {
    public:
        //[[nodiscard]] virtual std::vector<float> axis_states() noexcept = 0;

        //[[nodiscard]] virtual std::vector<bool> button_states() noexcept = 0;

        //[[nodiscard]] virtual std::vector<_native_input_event> poll_events() noexcept = 0;

        [[nodiscard]] virtual std::tuple<std::error_code, input_device_state>
        poll_state() noexcept = 0;
    };

} // namespace drako::engine::input

#endif // !DRAKO_INPUT_SYSTEM_TYPES_HPP_