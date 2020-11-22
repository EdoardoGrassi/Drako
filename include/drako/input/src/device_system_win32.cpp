#include "drako/input/device_system.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/logging.hpp"
#include "drako/input/input_system_utils.hpp"

#include <Xinput.h>

#include <cassert>
#include <iostream>
#include <vector>

namespace drako
{
    using _this = device_system;

    [[nodiscard]] constexpr bool _valid(const std::uint32_t h) noexcept
    {
        return h != 0; //std::numeric_limits<decltype(h)>::max();
    }

    [[nodiscard]] bool _xinput_valid_port(device_port dev)
    {
        return (dev >= 0) & (dev < XUSER_MAX_COUNT);
    }

    [[nodiscard]] float _xinput_process_thumb(SHORT raw) noexcept
    {
        return normalize_signed_axis(raw);
    }

    [[nodiscard]] float _xinput_process_trigger(BYTE raw) noexcept
    {
        return normalize_unsigned_axis(raw);
    }


    [[nodiscard]] std::vector<device_info> query_active_devices() noexcept
    {
        std::vector<device_info> result;

        for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            XINPUT_STATE state;
            if (::XInputGetState(i, &state) == ERROR_SUCCESS)
            {
                result.push_back({ .api = "XInput",
                    .description        = "Player " + std::to_string(i + 1) + " controller",
                    .port               = static_cast<device_port>(i) });
            }
        }
        return result;
    }

    [[nodiscard]] std::vector<device_info> query_all_devices() noexcept
    {
        std::vector<device_info> result;

        for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            result.push_back({ .api = "XInput",
                .description        = "Player " + std::to_string(i + 1) + " controller",
                .port               = static_cast<device_port>(i) });
        }
        return result;
    }


    [[nodiscard]] device_read_result read(const device_id id) noexcept
    {
        XINPUT_STATE xs = {};
        switch (const auto ec = ::XInputGetState(id, &xs); ec)
        {
            case ERROR_SUCCESS:
            {
                device_input_state s{};

                const auto& gp           = xs.Gamepad;
                s.axes[vaxis::xbox_ls_x] = _xinput_process_thumb(gp.sThumbLX);
                s.axes[vaxis::xbox_ls_y] = _xinput_process_thumb(gp.sThumbLY);
                s.axes[vaxis::xbox_rs_x] = _xinput_process_thumb(gp.sThumbRX);
                s.axes[vaxis::xbox_rs_y] = _xinput_process_thumb(gp.sThumbRY);
                s.axes[vaxis::xbox_lt]   = _xinput_process_trigger(gp.bLeftTrigger);
                s.axes[vaxis::xbox_rt]   = _xinput_process_trigger(gp.bRightTrigger);

                s.buttons[vkey::dpad_up]        = gp.wButtons & XINPUT_GAMEPAD_DPAD_UP;
                s.buttons[vkey::dpad_down]      = gp.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                s.buttons[vkey::dpad_left]      = gp.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                s.buttons[vkey::dpad_right]     = gp.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                s.buttons[vkey::xbox_a]         = gp.wButtons & XINPUT_GAMEPAD_A;
                s.buttons[vkey::xbox_b]         = gp.wButtons & XINPUT_GAMEPAD_B;
                s.buttons[vkey::xbox_x]         = gp.wButtons & XINPUT_GAMEPAD_X;
                s.buttons[vkey::xbox_y]         = gp.wButtons & XINPUT_GAMEPAD_Y;
                s.buttons[vkey::xbox_start]     = gp.wButtons & XINPUT_GAMEPAD_START;
                s.buttons[vkey::xbox_back]      = gp.wButtons & XINPUT_GAMEPAD_BACK;
                s.buttons[vkey::shoulder_left]  = gp.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
                s.buttons[vkey::shoulder_right] = gp.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
                s.buttons[vkey::thumb_left]     = gp.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
                s.buttons[vkey::thumb_right]    = gp.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;

                return { {}, s };
            }

            case ERROR_DEVICE_NOT_CONNECTED:
                //std::invoke(on_device_lost);
                [[fallthrough]];

            default:
                return { std::error_code(ec, std::system_category()), {} };
        }
    }


    _this::device_system(const configuration& config)
        : _config{ config }
        , _clock{}
        , _last_poll_time{ _clock.now() }
    {
    }

    _this::~device_system() noexcept
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

    void _this::enable(const device_port dev) noexcept
    {
        if (_xinput_valid_port(dev))
            _enabled[dev] = true;
    }

    void _this::disable(const device_port dev) noexcept
    {
        if (_xinput_valid_port(dev))
            _enabled[dev] = false;
    }

    void _this::attach_state_callback(device_port p, listener_id l, device_state_callback c) noexcept
    {
        assert(_valid(p));
        assert(_valid(l));
        assert(c != nullptr);

        _state_event[p].ids.push_back(p);
        _state_event[p].callbacks.push_back(c);
    }

    void _this::detach_state_callback(device_port p, listener_id l) noexcept
    {
        assert(_valid(p));
        assert(_valid(l));

        if (const auto found = std::find(
                std::cbegin(_state_event[p].ids), std::cend(_state_event[p].ids), l);
            found != std::cend(_state_event[p].ids))
        {
            const auto index = std::distance(std::cbegin(_state_event[p].ids), found);
            _state_event[p].ids.erase(found);
            _state_event[p].callbacks.erase(std::cbegin(_state_event[p].callbacks) + index);
        }
        else
            DRAKO_LOG_ERROR("Couldn't find matching listener");
    }

    void _this::attach_device_arrival_callback(listener_id l, device_change_callback c) noexcept
    {
        assert(_valid(l));
        assert(c != nullptr);

        _dev_arrival.listeners.push_back(l);
        _dev_arrival.callbacks.push_back(c);
    }

    void _this::detach_device_arrival_callback(listener_id l) noexcept
    {
        assert(_valid(l));

        if (const auto found = std::find(
                std::cbegin(_dev_arrival.listeners), std::cend(_dev_arrival.listeners), l);
            found != std::cend(_dev_arrival.listeners))
        {
            const auto index = std::distance(std::cbegin(_dev_arrival.listeners), found);
            _dev_arrival.listeners.erase(found);
            _dev_arrival.callbacks.erase(std::cbegin(_dev_arrival.callbacks) + index);
        }
        else
            DRAKO_LOG_ERROR("Couldn't find matching listener");
    }

    void _this::attach_device_removal_callback(listener_id l, device_change_callback c) noexcept
    {
        assert(_valid(l));
        assert(c != nullptr);

        _dev_removal.listeners.push_back(l);
        _dev_removal.callbacks.push_back(c);
    }

    void _this::detach_device_removal_callback(listener_id l) noexcept
    {
        assert(_valid(l));

        if (const auto found = std::find(
                std::cbegin(_dev_removal.listeners), std::cend(_dev_removal.listeners), l);
            found != std::cend(_dev_removal.listeners))
        {
            const auto index = std::distance(std::cbegin(_dev_removal.listeners), found);
            _dev_removal.listeners.erase(found);
            _dev_removal.callbacks.erase(std::cbegin(_dev_removal.callbacks) + index);
        }
        else
            DRAKO_LOG_ERROR("Couldn't find matching listener");
    }

    void _this::update() noexcept
    {
        if (const auto now = _clock.now();
            (now - _last_poll_time) > _config.devices_poll_rate)
        {
            _last_poll_time = now;
            std::cout << "[INFO] Device System: polling devices...\n";
            // TODO: impl
        }

        for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            if (!_enabled[i] | !_connected[i])
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

                    device_input_state result{};

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
                    for (auto c : _dev_removal.callbacks)
                        std::invoke(c, device_change_event{ .id = static_cast<device_id>(i) });
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

    void _this::set(device_port p, const device_output_state& s) noexcept
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

#if !defined(DRAKO_API_RELEASE)
    void _this::dbg_print_listeners() const
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

} // namespace drako