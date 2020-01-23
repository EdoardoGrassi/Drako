#pragma once
#ifndef DRAKO_HID_STANDARD_HPP
#define DRAKO_HID_STANDARD_HPP

#include <cstdint>

namespace drako
{
    // HID standard reference:
    // https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

    enum class hid_report_id : std::uint8_t;

    enum class hid_usage_page : std::uint16_t
    {
        UNDEFINED                = 0x00,
        GENERIC_DESKTOP_CONTROLS = 0x01,
        SIMULATION_CONTROLS      = 0x02,
        VR_CONTROLS              = 0x03,
        SPORT_CONTROLS           = 0x04,
        GAME_CONTROLS            = 0x05,
        GENERIC_DEVICE_CONTROLS  = 0x06,
        KEYBOARD_OR_KEYPAD       = 0x07,
        LEDS                     = 0x08,
        BUTTON                   = 0x09,
        PID_PAGE                 = 0x0F
    };

    enum class hid_usage_id : std::uint16_t
    {
        UNDEFINED = 0x00,
        POINTER   = 0x01,
        MOUSE     = 0x02,
        KEYBOARD  = 0x06
    };

} // namespace drako

#endif // !DRAKO_HID_STANDARD_HPP