#include "drako/input/device_system.hpp"

#include "drako/core/platform.hpp"
#include "drako/devel/logging.hpp"
#include "drako/input/input_utils.hpp"

#include <Xinput.h>

#include <cassert>
#include <iostream>
#include <vector>

namespace input
{
    [[nodiscard]] bool _xinput_valid_port(GamepadPlayerPort port)
    {
        return (port >= 0) && (port < XUSER_MAX_COUNT);
    }

    [[nodiscard]] float _xinput_process_thumb(SHORT raw) noexcept
    {
        return normalize_signed_axis(raw);
    }

    [[nodiscard]] float _xinput_process_trigger(BYTE raw) noexcept
    {
        return normalize_unsigned_axis(raw);
    }


    [[nodiscard]] std::vector<DeviceInstanceInfo> query_active_devices() noexcept
    {
        std::vector<DeviceInstanceInfo> result;

        for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            XINPUT_STATE state;
            if (::XInputGetState(i, &state) == ERROR_SUCCESS)
            {
                result.push_back({ .api = "XInput",
                    .description        = "Player " + std::to_string(i + 1) + " controller",
                    .port               = static_cast<GamepadPlayerPort>(i) });
            }
        }
        return result;
    }

    [[nodiscard]] std::vector<DeviceInstanceInfo> query_all_devices() noexcept
    {
        std::vector<DeviceInstanceInfo> result;

        for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            result.push_back({ .api = "XInput",
                .description        = "Player " + std::to_string(i + 1) + " controller",
                .port               = static_cast<GamepadPlayerPort>(i) });
        }
        return result;
    }


    [[nodiscard]] DeviceStateResult query_gamepad_state(const GamepadPlayerPort port) noexcept
    {
        assert(_xinput_valid_port(port));

        XINPUT_STATE xs = {};
        switch (const auto ec = ::XInputGetState(port, &xs); ec)
        {
            case ERROR_SUCCESS:
            {
                DeviceInputState s{};

                const auto& gp           = xs.Gamepad;
                s.axes[VAxis::xbox_ls_x] = _xinput_process_thumb(gp.sThumbLX);
                s.axes[VAxis::xbox_ls_y] = _xinput_process_thumb(gp.sThumbLY);
                s.axes[VAxis::xbox_rs_x] = _xinput_process_thumb(gp.sThumbRX);
                s.axes[VAxis::xbox_rs_y] = _xinput_process_thumb(gp.sThumbRY);
                s.axes[VAxis::xbox_lt]   = _xinput_process_trigger(gp.bLeftTrigger);
                s.axes[VAxis::xbox_rt]   = _xinput_process_trigger(gp.bRightTrigger);

                s.buttons[VKey::dpad_up]        = gp.wButtons & XINPUT_GAMEPAD_DPAD_UP;
                s.buttons[VKey::dpad_down]      = gp.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                s.buttons[VKey::dpad_left]      = gp.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                s.buttons[VKey::dpad_right]     = gp.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                s.buttons[VKey::xbox_a]         = gp.wButtons & XINPUT_GAMEPAD_A;
                s.buttons[VKey::xbox_b]         = gp.wButtons & XINPUT_GAMEPAD_B;
                s.buttons[VKey::xbox_x]         = gp.wButtons & XINPUT_GAMEPAD_X;
                s.buttons[VKey::xbox_y]         = gp.wButtons & XINPUT_GAMEPAD_Y;
                s.buttons[VKey::xbox_start]     = gp.wButtons & XINPUT_GAMEPAD_START;
                s.buttons[VKey::xbox_back]      = gp.wButtons & XINPUT_GAMEPAD_BACK;
                s.buttons[VKey::shoulder_left]  = gp.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
                s.buttons[VKey::shoulder_right] = gp.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
                s.buttons[VKey::thumb_left]     = gp.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
                s.buttons[VKey::thumb_right]    = gp.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;

                return { s, {} };
            }

            case ERROR_DEVICE_NOT_CONNECTED:
                //std::invoke(on_device_lost);
                [[fallthrough]];

            default:
                return { {}, std::error_code(ec, std::system_category()) };
        }
    }

    [[nodiscard]] DeviceEventsResult query_gamepad_events(const GamepadPlayerPort port) noexcept
    {
        assert(_xinput_valid_port(port));

        std::vector<NativeGamepadControlID> dw, up;

        XINPUT_KEYSTROKE xk = {};
        DWORD            ec;
        while ((ec = ::XInputGetKeystroke(port, {}, &xk)) == ERROR_SUCCESS)
        {
            // NOTE: for the time being we discard autorepeat events
            // (the ones marked with the flag XINPUT_KEYSTROKE_REPEAT).
            switch (xk.Flags)
            {
                case XINPUT_KEYSTROKE_KEYDOWN:
                    dw.push_back(xk.VirtualKey);
                    break;

                case XINPUT_KEYSTROKE_KEYUP:
                    up.push_back(xk.VirtualKey);
                    break;
            }
        }
        if (ec == ERROR_EMPTY) // read all the input reports
            return { .pressed = dw, .released = up, .ec = {} };
        else
            return { .pressed = dw, .released = up, .ec = std::error_code(ec, std::system_category()) };
    }

    std::string translate_native_code(NativeGamepadControlID c)
    {
        switch (c)
        {
            case VK_PAD_A: return "XBox (A)";
            case VK_PAD_B: return "XBox (B)";
            case VK_PAD_X: return "XBox (X)";
            case VK_PAD_Y: return "XBox (Y)";
            case VK_PAD_RSHOULDER: return "XBox (RS)";
            case VK_PAD_LSHOULDER: return "XBox (LS)";
            case VK_PAD_LTRIGGER: return "XBox (LT)";
            case VK_PAD_RTRIGGER: return "XBox (RT)";
            case VK_PAD_DPAD_UP: return "XBox (DPAD-U)";
            case VK_PAD_DPAD_DOWN: return "XBox (DPAD-D)";
            case VK_PAD_DPAD_LEFT: return "XBox (DPAD-L)";
            case VK_PAD_DPAD_RIGHT: return "XBox (DPAD-R)";
            case VK_PAD_START: return "XBox (START)";
            case VK_PAD_BACK: return "XBox (BACK)";
            case VK_PAD_LTHUMB_PRESS: return "XBox (LT-PRESS)";
            case VK_PAD_RTHUMB_PRESS: return "XBox (RT_PRESS)";
            default: return "Unknown";
        }
    }


    DeviceSystem::DeviceSystem(const Configuration& config)
        : _config{ config }
        , _clock{}
        , _last_poll_time{ _clock.now() }
    {
    }

    DeviceSystem::~DeviceSystem() noexcept
    {
        for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            if (!std::empty(_state_event[i].ids))
            {
                std::clog << "[WARNING] Device System [state event table]: "
                          << "some callback still not detached\n";
            }
        }

        if (!std::empty(_dev_arrival.listeners))
        {
            std::clog << "[WARNING] Device System [arrival event table]: "
                      << "some callback still not detached\n";
        }

        if (!std::empty(_dev_removal.listeners))
        {
            std::clog << "[WARNING] Device System [removal event table]: "
                      << "some callback still not detached\n";
        }
    }

    void DeviceSystem::enable(const GamepadPlayerPort p) noexcept
    {
        if (_xinput_valid_port(p))
            _enabled[p] = true;
    }

    void DeviceSystem::disable(const GamepadPlayerPort p) noexcept
    {
        if (_xinput_valid_port(p))
            _enabled[p] = false;
    }

    void DeviceSystem::attach_state_callback(GamepadPlayerPort p, HandlerID h, DeviceStateCallback c) noexcept
    {
        assert(p);
        assert(h);
        assert(c);

        _state_event[p].ids.push_back(h);
        _state_event[p].callbacks.push_back(c);
    }

    void DeviceSystem::detach_state_callback(GamepadPlayerPort p, HandlerID h) noexcept
    {
        assert(p);
        assert(h);

        if (const auto found = std::find(
                std::cbegin(_state_event[p].ids), std::cend(_state_event[p].ids), h);
            found != std::cend(_state_event[p].ids))
        {
            const auto index = std::distance(std::cbegin(_state_event[p].ids), found);
            _state_event[p].ids.erase(found);
            _state_event[p].callbacks.erase(std::cbegin(_state_event[p].callbacks) + index);
        }
        else
            DRAKO_LOG_ERROR("Couldn't find matching listener");
    }

    void DeviceSystem::attach_device_arrival_callback(GamepadPlayerPort p, HandlerID h, DeviceChangeCallback c) noexcept
    {
        assert(p);
        assert(h);
        assert(c);

        _dev_arrival.listeners.push_back(h);
        _dev_arrival.callbacks.push_back(c);
    }

    void DeviceSystem::detach_device_arrival_callback(GamepadPlayerPort p, HandlerID h) noexcept
    {
        assert(p);
        assert(h);

        if (const auto found = std::find(
                std::cbegin(_dev_arrival.listeners), std::cend(_dev_arrival.listeners), h);
            found != std::cend(_dev_arrival.listeners))
        {
            const auto index = std::distance(std::cbegin(_dev_arrival.listeners), found);
            _dev_arrival.listeners.erase(found);
            _dev_arrival.callbacks.erase(std::cbegin(_dev_arrival.callbacks) + index);
        }
        else
            DRAKO_LOG_ERROR("Couldn't find matching listener");
    }

    void DeviceSystem::attach_device_removal_callback(GamepadPlayerPort p, HandlerID h, DeviceChangeCallback c) noexcept
    {
        assert(p);
        assert(h);
        assert(c);

        _dev_removal.listeners.push_back(h);
        _dev_removal.callbacks.push_back(c);
    }

    void DeviceSystem::detach_device_removal_callback(GamepadPlayerPort p, HandlerID h) noexcept
    {
        assert(p);
        assert(h);

        if (const auto found = std::find(
                std::cbegin(_dev_removal.listeners), std::cend(_dev_removal.listeners), h);
            found != std::cend(_dev_removal.listeners))
        {
            const auto index = std::distance(std::cbegin(_dev_removal.listeners), found);
            _dev_removal.listeners.erase(found);
            _dev_removal.callbacks.erase(std::cbegin(_dev_removal.callbacks) + index);
        }
        else
            DRAKO_LOG_ERROR("Couldn't find matching listener");
    }

    void DeviceSystem::update() noexcept
    {
        if (const auto now = _clock.now();
            (now - _last_poll_time) > _config.devices_poll_rate)
        {
            _last_poll_time = now;
            std::cout << "[INFO] Device System: polling devices...\n";
            // TODO: impl
        }

        for (std::uint32_t i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            if (!_enabled[i] || !_connected[i])
                continue;

            XINPUT_STATE current_state = {};
            switch (const auto ec = ::XInputGetState(i, &current_state); ec)
            {
                case ERROR_SUCCESS:
                {
                    auto& last_state = _devs[i].last_state;
                    if (last_state.dwPacketNumber == current_state.dwPacketNumber)
                        break; // no updates needed

                    last_state = current_state;

                    DeviceInputState result{};

                    const auto& gp = last_state.Gamepad;
                    result.axes[0] = _xinput_process_thumb(gp.sThumbLX);
                    result.axes[1] = _xinput_process_thumb(gp.sThumbLY);
                    result.axes[2] = _xinput_process_thumb(gp.sThumbRX);
                    result.axes[3] = _xinput_process_thumb(gp.sThumbRY);
                    result.axes[4] = _xinput_process_trigger(gp.bLeftTrigger);
                    result.axes[5] = _xinput_process_trigger(gp.bRightTrigger);

                    result.buttons[0]  = gp.wButtons & XINPUT_GAMEPAD_DPAD_UP;
                    result.buttons[1]  = gp.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                    result.buttons[2]  = gp.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                    result.buttons[3]  = gp.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                    result.buttons[4]  = gp.wButtons & XINPUT_GAMEPAD_A;
                    result.buttons[5]  = gp.wButtons & XINPUT_GAMEPAD_B;
                    result.buttons[6]  = gp.wButtons & XINPUT_GAMEPAD_X;
                    result.buttons[7]  = gp.wButtons & XINPUT_GAMEPAD_Y;
                    result.buttons[8]  = gp.wButtons & XINPUT_GAMEPAD_START;
                    result.buttons[9]  = gp.wButtons & XINPUT_GAMEPAD_BACK;
                    result.buttons[10] = gp.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
                    result.buttons[11] = gp.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;

                    for (auto c : _state_event[i].callbacks)
                        std::invoke(c, result);
                    break;
                }

                case ERROR_DEVICE_NOT_CONNECTED:
                {
                    _connected[i] = false;
                    const DeviceChangeEvent context{ .id = DeviceID{ i } };
                    for (auto c : _dev_removal.callbacks)
                        std::invoke(c, context);
                    break;
                }

                default:
                {
                    const std::error_code e(ec, std::system_category());
                    std::clog << "[ERROR] Device System: " << e.message() << '\n';
                }
            }
        }
    }

    void DeviceSystem::set(GamepadPlayerPort p, const DeviceOutputState& s) noexcept
    {
        using _left  = decltype(XINPUT_VIBRATION::wLeftMotorSpeed);
        using _right = decltype(XINPUT_VIBRATION::wRightMotorSpeed);

        if (_xinput_valid_port(p))
        {
            XINPUT_VIBRATION vibs{
                .wLeftMotorSpeed = static_cast<_left>(
                    s.left_rumble * std::numeric_limits<_left>::max()),
                .wRightMotorSpeed = static_cast<_right>(
                    s.right_rumble * std::numeric_limits<_right>::max())
            };
            if (const auto ec = ::XInputSetState(p, &vibs);
                ec == ERROR_DEVICE_NOT_CONNECTED)
            {
                _connected[p] = false;
            }
        }
    }

#if !defined(NDEBUG)
    void DeviceSystem::debug_print_handlers() const
    {
        std::cout << "[INFO] Device System [subscribed state listeners]:\n";
        for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            std::cout << "\tXInput device " << i << '\n';
            for (const auto id : _state_event[i].ids)
                std::cout << '\t\t' << id << '\n';
        }
        std::cout << std::endl;
    }
#endif

} // namespace input