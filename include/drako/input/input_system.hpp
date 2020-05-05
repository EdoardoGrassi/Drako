#pragma once
#ifndef DRAKO_INPUT_SYSTEM_HPP_
#define DRAKO_INPUT_SYSTEM_HPP_

#include "drako/devel/logging.hpp"
#include "drako/input/input_system_types.hpp"

#include <cstdint>
#include <vector>

namespace drako::engine::input
{
    struct _native_input_event
    {
    };


    // Metadata for an input device.
    struct input_device_info
    {
        std::string native_interface;
        size_t      native_device_id;
    };
    [[nodiscard]] std::vector<input_device_info> query_input_devices() noexcept;


    // Provides a uniform control interface over physical input devices.
    class input_system
    {
    public:
        using device_id            = std::uint32_t;
        using callback_listener_id = std::uint32_t;

        using device_state_callback = void (*)(const input_device_state&) noexcept;
        using device_event_callback = void (*)(const input_device_event&) noexcept;

        explicit input_system() = default;

        input_system(const input_system&) = delete;
        input_system& operator=(const input_system&) = delete;

        input_system(input_system&&) = delete;
        input_system& operator=(input_system&&) = delete;


        // Create a new device instance.
        [[nodiscard]] device_id create_device(const input_device_info& info) noexcept;


        [[nodiscard]] callback_listener_id
        insert_state_callback(device_id d, device_state_callback cb) noexcept;


        void remove_state_callback(device_id d, callback_listener_id cl) noexcept;


        // Destroy a device instance.
        void destroy_device(device_id device) noexcept;


        // Run a single update cycle of all active devices.
        void update() noexcept;

#if defined(DRAKO_DEBUG)
        // Prints list of registered listeners.
        void dbg_print_listeners() const;
#endif

    private:
        // event produced by state changes of the controls
        struct _device_state_event
        {
            std::vector<callback_listener_id>  listeners;
            std::vector<device_state_callback> callbacks;
        };

        // event produced by configuration changes of the device
        struct _device_config_event
        {
        };

        std::vector<device_id>                               _device_ids;
        std::vector<std::unique_ptr<input_device_interface>> _device_interfaces;
        std::vector<input_device_layout>                     _device_layouts;

        std::vector<callback_listener_id>  _state_listeners;
        std::vector<device_state_callback> _state_callbacks;

        std::vector<callback_listener_id>  _event_listeners;
        std::vector<device_event_callback> _event_callbacks;

        [[nodiscard]] static device_id _new_device_id() noexcept
        {
            static device_id last_generated = 0;
            return ++last_generated;
        }

        [[nodiscard]] callback_listener_id _new_listener_id() noexcept
        {
            static callback_listener_id last_generated = 0;
            return ++last_generated;
        }
    };

} // namespace drako::engine::input

#endif // !DRAKO_INPUT_SYSTEM_HPP_