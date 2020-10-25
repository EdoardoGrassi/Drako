#pragma once
#ifndef DRAKO_NATIVE_MOUSE_DEVICE_HPP
#define DRAKO_NATIVE_MOUSE_DEVICE_HPP

#include <chrono>
#include <vector>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/math/vector2.hpp"
#include "drako/system/desktop_window.hpp"

namespace drako::sys
{
    enum class mouse_action : uint8_t
    {
        DK_LBTN_PRESS,
        DK_LBTN_RELEASE,
        DK_LBTN_DOUBLE_CLK,

        DK_RBTN_PRESS,
        DK_RBTN_RELEASE,
        DK_RBTN_DOUBLE_CLK,

        DK_MBTN_PRESS,
        DK_MBTN_RELEASE,
        DK_MBTN_DOUBL_CLK

        // DK_WHEEL_MOVE
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

        DRAKO_NODISCARD constexpr ivec2
        cursor_local_position() const noexcept { return _position; }

        DRAKO_NODISCARD constexpr auto
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


    // OS-dependent interface with mouse device.
    //
    class native_mouse_device
    {
    public:
        // using mouse_event_handler = void(*)(const mouse_action_event&);

#if defined(DRAKO_PLT_WIN32)
        explicit native_mouse_device(const desktop_window& context) noexcept
            : _context(context)
        {
        }
#endif

        void poll_state() noexcept;

        DRAKO_NODISCARD std::vector<mouse_event> poll_events() noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)
        const desktop_window& _context;
#endif
    };
} // namespace drako::sys

#endif // !DRAKO_NATIVE_MOUSE_DEVICE_HPP