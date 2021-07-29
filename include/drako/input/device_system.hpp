#pragma once
#ifndef DEVICE_SYSTEM_HPP
#define DEVICE_SYSTEM_HPP

#include "drako/core/platform.hpp"
#include "drako/core/typed_handle.hpp"
#include "drako/input/device_types.hpp"

#if defined(_WIN32)
#include <Xinput.h>
#endif

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace input
{
    /// @brief List of currently connected devices.
    [[nodiscard]] std::vector<DeviceInstanceInfo> query_active_devices() noexcept;

    /// @brief List of registered devices.
    [[nodiscard]] std::vector<DeviceInstanceInfo> query_all_devices() noexcept;

    struct DeviceStateResult
    {
        DeviceInputState state;
        std::error_code  ec;
    };
    /// @breif Read the current state of a device.
    [[nodiscard]] DeviceStateResult query_gamepad_state(GamepadPlayerPort) noexcept;

    struct DeviceEventsResult
    {
        std::vector<NativeGamepadControlID> pressed;
        std::vector<NativeGamepadControlID> released;
        std::error_code                     ec;
    };
    /// @brief Read the events generated from a device.
    [[nodiscard]] DeviceEventsResult query_gamepad_events(GamepadPlayerPort) noexcept;

    [[nodiscard]] std::string translate_native_code(NativeGamepadControlID);


    /// @brief Uniform control interface over physical input devices.
    class DeviceSystem
    {
    public:
        //DRAKO_DEFINE_TYPED_ID(HandlerID, std::uint32_t);
        using HandlerID = std::uint32_t;

        using DeviceChangeCallback = void (*)(const DeviceChangeEvent&) noexcept;
        using DeviceStateCallback  = void (*)(const DeviceInputState&) noexcept;


        struct Configuration
        {
            std::chrono::milliseconds devices_poll_rate;
        };


        //explicit device_system();
        explicit DeviceSystem(const Configuration&);
        ~DeviceSystem() noexcept;

        DeviceSystem(const DeviceSystem&) = delete;
        DeviceSystem& operator=(const DeviceSystem&) = delete;

        // Enable communications with a specific physical device.
        void enable(GamepadPlayerPort) noexcept;

        // Disable communications with a specific physical device.
        void disable(GamepadPlayerPort) noexcept;

        void attach_device_arrival_callback(GamepadPlayerPort, HandlerID, DeviceChangeCallback) noexcept;
        void detach_device_arrival_callback(GamepadPlayerPort, HandlerID) noexcept;

        void attach_device_removal_callback(GamepadPlayerPort, HandlerID, DeviceChangeCallback) noexcept;
        void detach_device_removal_callback(GamepadPlayerPort, HandlerID) noexcept;

        // Create a listener of device state notifications.
        void attach_state_callback(GamepadPlayerPort, HandlerID, DeviceStateCallback) noexcept;

        // Remove a listener of device state notifications.
        void detach_state_callback(GamepadPlayerPort, HandlerID) noexcept;

        void set(GamepadPlayerPort, const DeviceOutputState&) noexcept;

        // Run a single update cycle of all active devices.
        void update() noexcept;

#if !defined(DRAKO_API_RELEASE)
        // Prints list of registered listeners.
        void debug_print_handlers() const;
#endif

    private:
        /*
        struct _device_table_t
        {
            std::vector<device_id>                   ids;
            std::vector<std::unique_ptr<_interface>> interfaces;
            std::vector<_layout>                     layouts;
        } _devices;
        */

        struct _xinput_device
        {
            XINPUT_STATE last_state;
            //DWORD        user_index;
        };
        _xinput_device               _devs[XUSER_MAX_COUNT];
        std::bitset<XUSER_MAX_COUNT> _connected; // whether the controller is physically connected
        std::bitset<XUSER_MAX_COUNT> _enabled;   // whethet the controller is virtually active

        const Configuration          _config;
        std::chrono::steady_clock    _clock;
        decltype(_clock)::time_point _last_poll_time; // time point of the last poll of connected devices

        struct _state_listeners_table_t
        {
            std::vector<HandlerID>           ids;
            std::vector<DeviceStateCallback> callbacks;
        } _state_event[XUSER_MAX_COUNT];

        struct _removal_listeners_table_t
        {
            std::vector<HandlerID>            listeners;
            std::vector<DeviceChangeCallback> callbacks;
        } _dev_arrival;

        struct _arrival_listeners_table_t
        {
            std::vector<HandlerID>            listeners;
            std::vector<DeviceChangeCallback> callbacks;
        } _dev_removal;
    };

} // namespace input

#endif // !DEVICE_SYSTEM_HPP