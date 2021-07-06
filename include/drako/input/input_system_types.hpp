#pragma once
#ifndef DRAKO_INPUT_SYSTEM_TYPES_HPP
#define DRAKO_INPUT_SYSTEM_TYPES_HPP

#include "drako/core/typed_handle.hpp"
#include "drako/file_formats/dson/dson.hpp"
#include "drako/input/device_system_types.hpp"

#include <yaml-cpp/yaml.h>

#include <array>
#include <bitset>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace drako::input
{
    /// @brief Unique identifier of a generated event.
    DRAKO_DEFINE_TYPED_ID(EventID, std::uint32_t); // strengthened

    /// @brief Unique identifier of an input binding location.
    DRAKO_DEFINE_TYPED_ID(BindingID, std::uint32_t);

    /// @brief Virtual control that carries a boolean value.
    DRAKO_DEFINE_TYPED_ID(BooleanControlID, std::uint32_t);

    /// @brief Virtual control that carries a continuous value.
    DRAKO_DEFINE_TYPED_ID(AxisControlID, std::uint32_t);

    struct GamepadButtonBinding
    {
        std::string      name;
        BindingID        id;
        GamepadButtonID  button;
        BooleanControlID control;
    };

    struct GamepadAxisBinding
    {
        std::string   name;
        BindingID     id;
        GamepadAxisID axis;
        AxisControlID control;
    };

    struct KeyboardButtonBinding
    {
        std::string      name;
        BindingID        id;
        KeyboardKeyID    key;
        BooleanControlID control;
    };

    struct KeyboardAxisBinding
    {
        std::string   name;
        BindingID     id1;
        BindingID     id2;
        KeyboardKeyID axis1;
        KeyboardKeyID axis2;
        AxisControlID control;
    };

    struct InputAxisInfo
    {
        std::string name;
        float       range_min;
        float       range_max;
        float       deadzone_min;
        float       deadzone_max;
        bool        invert;
    };

    struct InputButtonInfo
    {
        std::string name;
    };

    struct VirtualAxisBinding
    {
        std::uint8_t  id;
        std::uint16_t action;
    };

    struct VirtualButtonBinding
    {
    };

    struct Gesture
    {
        std::string      name;
        BooleanControlID control;
        EventID          event;
    };


    struct ActionMap
    {
        std::vector<Gesture>               gestures;
        std::vector<GamepadButtonBinding>  gamepad_button_bindings;
        std::vector<GamepadAxisBinding>    gamepad_axis_bindings;
        std::vector<KeyboardButtonBinding> keyboard_button_bindings;
        std::vector<KeyboardAxisBinding>   keyboard_axis_bindings;
    };

    const dson::DOM& operator>>(const dson::DOM&, ActionMap&);
    dson::DOM&       operator<<(dson::DOM&, const ActionMap&);

    const YAML::Node& operator>>(const YAML::Node&, ActionMap&);
    YAML::Node&       operator<<(YAML::Node&, const ActionMap&);



    /// @brief Handles recording of input events
    class InputTrace
    {
    public:
        struct InputTraceEvent
        {
            //std::duration timestamp;
        };

        explicit InputTrace();

    private:
        std::vector<InputTraceEvent> _events;
    };

} // namespace drako::input

#endif // !DRAKO_INPUT_SYSTEM_TYPES_HPP