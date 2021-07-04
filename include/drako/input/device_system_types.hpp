#pragma once
#ifndef DRAKO_DEVICE_SYSTEM_TYPES_HPP
#define DRAKO_DEVICE_SYSTEM_TYPES_HPP

#include "drako/core/platform.hpp"
#include "drako/core/typed_handle.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#include <Xinput.h> // VK_PAD_* codes
#endif

#include <array>
#include <bitset>

namespace drako
{
    DRAKO_DEFINE_TYPED_ID(DeviceID, std::uint32_t);

    using NativeGamepadControlID = std::uint32_t;

    using GamepadPlayerPort = std::uint32_t;

    // Metadata for an input device.
    struct DeviceInstanceInfo
    {
        std::string       api;
        std::string       description;
        GamepadPlayerPort port;
    };

    // Describes the current state of an input device.
    struct DeviceInputState
    {
        std::bitset<14>      buttons;
        std::array<float, 6> axes;
    };

    // Describe  av event at system level.
    struct DeviceChangeEvent
    {
        DeviceID id;
    };

    struct DeviceOutputState
    {
        float left_rumble;
        float right_rumble;
    };

    /// @brief Unique identifier of a physical axis on a gamepad.
    using GamepadAxisID = std::uint32_t;

    /// @brief Unique identifier of a physical button on a gamepad.
    using GamepadButtonID = std::uint32_t;

    /// @brief Unique identifier of a physical key on a keyboard.
    using KeyboardKeyID = std::uint32_t;

    /// @brief Platform indipendent gamepad axes names.
    enum VAxis : std::uint32_t
    {
        left_thumb_horizontal,
        xbox_ls_x = left_thumb_horizontal,

        left_thumb_vertical,
        xbox_ls_y = left_thumb_vertical,

        right_thumb_horizontal,
        xbox_rs_x = right_thumb_horizontal,

        right_thumb_vertical,
        xbox_rs_y = right_thumb_vertical,

        trigger_left,
        xbox_lt = trigger_left,

        trigger_right,
        xbox_rt = trigger_right,
    };


    /// @brief Platform indipendent gamepad buttons names.
    enum VKey : std::uint32_t
    {
        /// @brief Upward direction on the frontal d-pad.
        dpad_up = 0u,

        /// @brief Downard direction on the frontal d-pad.
        dpad_down,

        /// @brief Left direction on the frontal d-pad.
        dpad_left,

        /// @brief Right direction on the frontal d-pad.
        dpad_right,

        /// @brief Topmost button of the 4 frontal buttons.
        ///
        /// Maps to [Y] on XBOX controllers.
        /// Maps to [TRIANGLE] on PS controllers.
        button_north,
        xbox_y      = button_north,
        ps_triangle = button_north,

        /// @brief Lowest button of the 4 frontal buttons.
        ///
        /// Maps to [A] on XBOX controllers.
        /// Maps to [CROSS] on PS controllers.
        button_south,
        xbox_a   = button_south,
        ps_cross = button_south,

        /// @brief Leftmost button of the 4 frontal buttons.
        ///
        /// Maps to [X] on XBOX controllers.
        /// Maps to [SQUARE] on PS controllers.
        button_west,
        xbox_x    = button_west,
        ps_square = button_west,

        /// @brief Rightmost button of the 4 frontal buttons.
        ///
        /// Maps to [B] on XBOX controllers.
        /// Maps to [CIRCLE] on PS controllers.
        button_east,
        xbox_b    = button_east,
        ps_circle = button_east,

        thumb_left,
        thumb_right,

        shoulder_left,
        shoulder_right,

        /// @brief Special button at the left of the central main button.
        ///
        /// Maps to [BACK] on XBOX controllers.
        /// Maps to [SHARE] on PS controllers.
        special_left,
        xbox_back = special_left,
        ps_share  = special_left,

        /// @brief Special button at the right of the central main button.
        ///
        /// Maps to [START] on XBOX controllers.
        /// Maps to [MENU] on PS controllers.
        special_right,
        xbox_start = special_right,
        ps_menu    = special_right,
    };
} // namespace drako

#endif // !DRAKO_DEVICE_SYSTEM_TYPES_HPP