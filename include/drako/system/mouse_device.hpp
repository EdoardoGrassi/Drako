#pragma once
#ifndef DRAKO_MOUSE_DEVICE_HPP
#define DRAKO_MOUSE_DEVICE_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/math/vector2.hpp"
#include "drako/system/desktop_window.hpp"

#include <chrono>
#include <vector>

namespace drako::sys
{
    enum class mouse_action : uint8_t
    {
        left_btn_press,   // button press
        left_btn_release, // button release
        left_btn_double,  // double click

        mid_btn_press,   // button press
        mid_btn_release, // button release
        mid_btn_double,  // double click

        right_btn_press,   // button press
        right_btn_release, // button release
        right_btn_double,  // double click
    };

    // Event generated when the user performs an action with the mouse.
    //
    class mouse_event
    {
    public:
        explicit constexpr mouse_event(mouse_action interaction,
            ivec2                                   position,
            std::chrono::milliseconds               time) noexcept
            : _timestamp(time)
            , _position(position)
            , _action(interaction)
        {
        }

        [[nodiscard]] constexpr ivec2
        cursor_local_position() const noexcept { return _position; }

        [[nodiscard]] constexpr auto
        local_timestamp() const noexcept { return _timestamp; }

    private:
        std::chrono::milliseconds _timestamp; // Timestamp of the event.
        ivec2                     _position;  // Screen position of the cursor.
        mouse_action              _action;    // Action performed by the user.
    };


    struct mouse_wheel_event
    {
    public:
        std::chrono::milliseconds timestamp;
        ivec2                     position;
        float                     delta;
    };


    // Event generated at every update that tracks the state of the mouse.
    //
    struct mouse_tracking_event
    {
    };


    /// @brief OS interface with mouse device.
    class mouse_device
    {
    public:
        // using mouse_event_handler = void(*)(const mouse_action_event&);

#if defined(DRAKO_PLT_WIN32)
        explicit mouse_device(const desktop_window& context) noexcept
            : _context(context)
        {
        }
#endif

        void poll_state() noexcept;

        [[nodiscard]] std::vector<mouse_event> poll_events() noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)
        const desktop_window& _context;
#endif
    };
} // namespace drako::sys

#endif // !DRAKO_MOUSE_DEVICE_HPP