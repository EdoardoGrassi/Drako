#pragma once
#ifndef DRAKO_NATIVE_KEYBOARD_DEVICE_HPP
#define DRAKO_NATIVE_KEYBOARD_DEVICE_HPP

#include "drako/core/system/native_window.hpp"

#include <chrono>
#include <vector>

namespace drako::sys
{
    // Device-dependent code that identifies a physical key on the keyboard.
    enum class keyboard_scan_code : std::uint16_t
    {
        UNDEFINED = std::numeric_limits<std::underlying_type_t<keyboard_scan_code>>::max()
    };

    // System-dependent code that identifies a physical key on the keyboard.
    enum class keyboard_virt_code : std::uint16_t
    {
        UNDEFINED = std::numeric_limits<std::underlying_type_t<keyboard_virt_code>>::max()
    };

    // Translates scan codes to virtual codes.
    DRAKO_NODISCARD keyboard_virt_code map(keyboard_scan_code sc) noexcept;

    // Translates virtual codes to scan codes.
    DRAKO_NODISCARD keyboard_scan_code map(keyboard_virt_code vc) noexcept;

    class keyboard_event
    {
    public:
        enum class action : std::uint8_t
        {
            KEY_PRESS,
            KEY_RELEASE
        };

    private:
        action                    _action;
        std::chrono::milliseconds _timestamp;
        keyboard_scan_code        _scan_code;
        keyboard_virt_code        _virt_code;
    };

    // Low level platform-independent interface to a keyboard-like input device.
    class native_keyboard_device
    {
    public:
        explicit native_keyboard_device() = default;

        native_keyboard_device(const native_keyboard_device&) = delete;
        native_keyboard_device& operator=(const native_keyboard_device&) = delete;

        DRAKO_NODISCARD std::vector<keyboard_event> poll_events() noexcept;

        void simulate(const keyboard_event&) noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)
        const native_window& _context;
#endif
    };

} // namespace drako::sys

#endif // !DRAKO_NATIVE_KEYBOARD_DEVICE_HPP