#pragma once
#include "drako/input/input_system.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/logging.hpp"

#if defined(DRAKO_PLT_WIN32)
#include "drako/input/xinput_device_interface_win32.hpp"
#endif

#include <vector>

namespace drako::engine::input
{
    [[nodiscard]] inline constexpr bool _is_valid(
        const input_system::device_id id) noexcept
    {
        return id != 0;
    }

    [[nodiscard]] inline constexpr bool _is_valid(
        const input_system::callback_listener_id id) noexcept
    {
        return id != 0;
    }

    [[nodiscard]] std::vector<input_device_info> query_input_devices() noexcept
    {
#if defined(DRAKO_PLT_WIN32)
        std::vector<input_device_info> result;

        const auto xinput_dev_indexes = xinput_active_gamepad_indexes();
        for (const auto index : xinput_dev_indexes)
        {
            input_device_info info = {};
            info.native_interface  = "XInput";
            info.native_device_id  = index;
            result.push_back(info);
        }

        return result;
#else
#error Platform currently not supported
#endif
    }


    [[nodiscard]] input_system::device_id
    input_system::create_device(const input_device_info& info) noexcept
    {
#if defined(DRAKO_PLT_WIN32)
        if (info.native_interface == "XInput")
        {
            const auto id = _new_device_id();
            _device_ids.push_back(id);

            auto instance = std::make_unique<xinput_device_interface>(info.native_device_id);
            _device_interfaces.push_back(std::move(instance));

            return id;
        }
        else
        {
            DRAKO_LOG_ERROR("Unrecognized device interface");
            std::exit(EXIT_FAILURE);
        }
#else
#error Platform currently not supported
#endif
    }

    [[nodiscard]] input_system::callback_listener_id
    input_system::insert_state_callback(device_id d, device_state_callback cb) noexcept
    {
        DRAKO_ASSERT(_is_valid(d));

        const auto id = _new_listener_id();
        _state_listeners.push_back(id);
        _state_callbacks.push_back(cb);
        return id;
    }

    void input_system::remove_state_callback(device_id d, callback_listener_id cl) noexcept
    {
        DRAKO_ASSERT(_is_valid(d));
        DRAKO_ASSERT(_is_valid(cl));

        if (const auto found = std::find(
                std::cbegin(_state_listeners), std::cend(_state_listeners), cl);
            found != std::cend(_state_listeners))
        {
            const auto index = std::distance(std::cbegin(_state_listeners), found);
            _state_listeners.erase(found);
            _state_callbacks.erase(std::cbegin(_state_callbacks) + index);
        }
        else
        {
            DRAKO_LOG_ERROR("Couldn't find matching listener");
        }
    }

    void input_system::update() noexcept
    {
        using _index_type = decltype(std::size(_device_ids));

        for (_index_type i = 0; i < std::size(_device_ids); ++i)
        {
            if (const auto [err, state] = _device_interfaces[i]->poll_state(); err)
            {
                for (const auto& callback : _state_callbacks)
                    std::invoke(callback, state);
            }

            /*
            const auto events = _device_interfaces[i]->poll_events();
            for (const auto& callback : _event_callbacks)
                for (const auto event : events)
                    std::invoke(callback, event);
                    */
        }
    }

#if defined(DRAKO_DEBUG)
    void input_system::dbg_print_listeners() const
    {
        using _index_t = decltype(std::size(_state_callbacks));
        for (_index_t i = 0; i < std::size(_state_callbacks); ++i)
        {
        }
    }
#endif

} // namespace drako::engine