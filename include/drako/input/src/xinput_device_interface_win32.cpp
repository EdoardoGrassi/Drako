#pragma once

#include "drako/input/xinput_device_interface_win32.hpp"

namespace drako::engine
{
    [[nodiscard]] float _xinput_process_thumb(SHORT raw) noexcept
    {
        const auto normalized = normalize_bidirectional_axis(raw);
        return deadzone(normalized, 0.1f, 0.9f);
    }

    [[nodiscard]] float _xinput_process_trigger(BYTE raw) noexcept
    {
        const auto normalized = normalize_unidirectional_axis(raw);
        return deadzone(normalized, 0.1f, 0.9f);
    }

    [[nodiscard]] bool _xinput_process_button(WORD raw, WORD mask) noexcept
    {
        return (raw & mask) == 0;
    }


    [[nodiscard]] std::vector<size_t> xinput_active_gamepad_indexes() noexcept
    {
        std::vector<size_t> result;
        result.reserve(XUSER_MAX_COUNT);

        for (size_t i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            XINPUT_STATE state;
            if (::XInputGetState(i, &state) == ERROR_SUCCESS)
                result.push_back(i);
        }
        return result;
    }

    [[nodiscard]] std::tuple<std::error_code, input_device_state>
    xinput_device_interface::poll_state() noexcept
    {
        DRAKO_ASSERT(_user_index < XUSER_MAX_COUNT, "Range required by XInput API");

        XINPUT_STATE current_state = {};
        if (const auto err = ::XInputGetState(_user_index, &current_state);
            err == ERROR_SUCCESS)
        {
            if (_last_state.dwPacketNumber == current_state.dwPacketNumber)
                return {};

            _last_state = current_state;

            input_device_state result = {};

            const auto& gp = _last_state.Gamepad;
            result.axes[0] = _xinput_process_thumb(gp.sThumbLX);
            result.axes[1] = _xinput_process_thumb(gp.sThumbLY);
            result.axes[2] = _xinput_process_thumb(gp.sThumbRX);
            result.axes[3] = _xinput_process_thumb(gp.sThumbRY);
            result.axes[4] = _xinput_process_trigger(gp.bLeftTrigger);
            result.axes[5] = _xinput_process_trigger(gp.bRightTrigger);

            result.buttons[0]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_DPAD_UP);
            result.buttons[1]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
            result.buttons[2]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
            result.buttons[3]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
            result.buttons[4]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_A);
            result.buttons[5]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_B);
            result.buttons[6]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_X);
            result.buttons[7]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_Y);
            result.buttons[8]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_START);
            result.buttons[9]  = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_BACK);
            result.buttons[10] = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
            result.buttons[11] = _xinput_process_button(gp.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);

            return { std::error_code{}, result };
        }
        else
        {
            if (err == ERROR_DEVICE_NOT_CONNECTED)
                return { std::error_code(err, std::system_category()), {} };
            else
                return { std::error_code(err, std::system_category()), {} };
        }
    }

    void xinput_device_interface::set_vibration(float left, float right) noexcept
    {
        using _l_vib_t = decltype(XINPUT_VIBRATION::wLeftMotorSpeed);
        using _r_vib_t = decltype(XINPUT_VIBRATION::wRightMotorSpeed);

        XINPUT_VIBRATION vibs = {};
        vibs.wLeftMotorSpeed  = static_cast<_l_vib_t>(left * std::numeric_limits<_l_vib_t>::max());
        vibs.wRightMotorSpeed = static_cast<_r_vib_t>(right * std::numeric_limits<_r_vib_t>::max());
        if (const auto err = ::XInputSetState(_user_index, &vibs);
            err == ERROR_SUCCESS)
            return;
        else if (err == ERROR_DEVICE_NOT_CONNECTED)
            return;
    }

} // namespace drako::engine