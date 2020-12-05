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
    enum class ScanCode : std::uint16_t
    {
        undefined = std::numeric_limits<std::underlying_type_t<ScanCode>>::max()
    };

    [[nodiscard]] inline std::string to_string(ScanCode code)
    {
        switch (code)
        {
            case ScanCode::undefined: return "UNDEFINED";
        }
        return "UNKNOWN";
    }

    // System-dependent code that identifies a physical key on the keyboard.
    enum class VirtCode : std::uint16_t
    {
        undefined = std::numeric_limits<std::underlying_type_t<VirtCode>>::max()
    };

    [[nodiscard]] inline std::string to_string(VirtCode code)
    {
        switch (code)
        {
            case VirtCode::undefined: return "UNDEFINED";
        }
        return "UNKNOWN";
    }

    // Translates scan codes to virtual codes.
    [[nodiscard]] VirtCode map(ScanCode sc) noexcept;

    // Translates virtual codes to scan codes.
    [[nodiscard]] ScanCode map(VirtCode vc) noexcept;


    // Low level platform-independent interface to a keyboard-like input device.
    class Keyboard
    {
    public:
        struct Event
        {
        public:
            enum class Action : std::uint8_t
            {
                key_press,
                key_release
            };

            std::chrono::milliseconds timestamp;
            ScanCode                  scan_code;
            VirtCode                  virt_code;
            Action                    action;
        };

        explicit Keyboard(const desktop_window& context) noexcept
            : _context{ context }
        {
        }

        Keyboard(const Keyboard&) = delete;
        Keyboard& operator=(const Keyboard&) = delete;

        [[nodiscard]] std::vector<Event> poll_events() noexcept;

        void simulate(const Event&) noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)
        const desktop_window& _context;
#endif
    };

    [[nodiscard]] inline std::string to_string(Keyboard::Event::Action a)
    {
        using _code = Keyboard::Event::Action;
        switch (a)
        {
            case _code::key_press: return "KEY_PRESS";
            case _code::key_release: return "KEY_RELEASE";
        }
        return "UNKNOWN";
    }

} // namespace drako::sys

#endif // !DRAKO_KEYBOARD_DEVICE_HPP