#pragma once
#ifndef DRAKO_INPUT_PORT_HPP
#define DRAKO_INPUT_PORT_HPP

#include <cstdint>

namespace drako::engine
{
    enum class InputPortType
    {
        BUTTON,
        AXIS_1D,
        AXIS_2D
    };

    // STRUCT
    // Channel used to receive data from an HID device.
    struct InputPort final
    {
        InputPortType type;

        uint32_t mask;

        uint16_t byte_offset;
    };

    // STRUCT
    // Channel used to send data to an HID device.
    struct OutputPort final
    {
    };

    // STRUCT
    // Channel used to configure an HID device.
    struct FeaturePort final
    {
    };

    struct InputStateReport
    {
        uint8_t btns[8];
    };
} // namespace drako::engine

#endif // !DRAKO_INPUT_PORT_HPP