#pragma once
#ifndef DRAKO_KEYBOARD_DEVICE_HPP
#define DRAKO_KEYBOARD_DEVICE_HPP

#include "drako/system/desktop_window.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace drako::sys
{
    // Device-dependent code that identifies a physical key on the keyboard.
    enum class keyboard_scan_code : std::uint16_t
    {
        undefined = std::numeric_limits<std::underlying_type_t<keyboard_scan_code>>::max()
    };

    std::string to_string(keyboard_scan_code code)
    {
        using _code = keyboard_scan_code;
        switch (code)
        {
            case _code::undefined: return "UNDEFINED";
        }
        return "UNKNOWN";
    }

    // System-dependent code that identifies a physical key on the keyboard.
    enum class keyboard_virt_code : std::uint16_t
    {
        undefined = std::numeric_limits<std::underlying_type_t<keyboard_virt_code>>::max()
    };

    std::string to_string(keyboard_virt_code code)
    {
        using _code = keyboard_virt_code;
        switch (code)
        {
            case _code::undefined: return "UNDEFINED";
        }
        return "UNKNOWN";
    }

    // Translates scan codes to virtual codes.
    [[nodiscard]] keyboard_virt_code map(keyboard_scan_code sc) noexcept;

    // Translates virtual codes to scan codes.
    [[nodiscard]] keyboard_scan_code map(keyboard_virt_code vc) noexcept;

    struct keyboard_event
    {
    public:
        enum class action_code : std::uint8_t
        {
            key_press,
            key_release
        };

        action_code               action;
        std::chrono::milliseconds timestamp;
        keyboard_scan_code        scan_code;
        keyboard_virt_code        virt_code;
    };

    std::string to_string(keyboard_event::action_code code)
    {
        using _code = keyboard_event::action_code;
        switch (code)
        {
            case _code::key_press: return "KEY_PRESS";
            case _code::key_release: return "KEY_RELEASE";
        }
        return "UNKNOWN";
    }

    // Low level platform-independent interface to a keyboard-like input device.
    class keyboard_device
    {
    public:
        explicit keyboard_device(const desktop_window& context) noexcept
            : _context { context }
        {
        }

        keyboard_device(const keyboard_device&) = delete;
        keyboard_device& operator=(const keyboard_device&) = delete;

        [[nodiscard]] std::vector<keyboard_event> poll_events() noexcept;

        void simulate(const keyboard_event&) noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)
        const desktop_window& _context;
#endif
    };

} // namespace drako::sys

#endif // !DRAKO_KEYBOARD_DEVICE_HPP