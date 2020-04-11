#pragma once
#ifndef DRAKO_HID_STANDARD_HPP_
#define DRAKO_HID_STANDARD_HPP_

#include <cstdint>
#include <iostream>

namespace drako::sys::hid
{
    // HID standard reference:
    // https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

    enum class usage_page : std::uint16_t
    {
        undefined                = 0x00,
        generic_desktop_controls = 0x01,
        simulation_controls      = 0x02,
        vr_controls              = 0x03,
        sport_controls           = 0x04,
        game_controls            = 0x05,
        generic_device_controls  = 0x06,
        keyboard_or_keypad       = 0x07,
        leds                     = 0x08,
        button                   = 0x09,
        pid_page                 = 0x0F
    };

    std::ostream& operator<<(std::ostream& os, usage_page up)
    {
        return os << static_cast<std::underlying_type_t<usage_page>>(up);
    }


    enum class usage_id : std::uint16_t
    {
        undefined = 0x00,
        pointer   = 0x01,
        mouse     = 0x02,
        keyboard  = 0x06
    };

    std::ostream& operator<<(std::ostream& os, usage_id ui)
    {
        return os << static_cast<std::underlying_type_t<usage_id>>(ui);
    }


} // namespace drako::sys::hid

#endif // !DRAKO_HID_STANDARD_HPP_