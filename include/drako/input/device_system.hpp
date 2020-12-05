#pragma once
#ifndef DRAKO_DEVICE_SYSTEM_HPP
#define DRAKO_DEVICE_SYSTEM_HPP

#include "drako/core/platform.hpp"
#include "drako/devel/logging.hpp"
#include "drako/input/device_system_types.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Xinput.h>
#endif

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace drako
{
    using device_port = std::uint32_t;

    enum class player
    {
        user1,
        user2,
        user3,
        user4
    };


    [[nodiscard]] std::vector<device_info> query_active_devices() noexcept;

    [[nodiscard]] std::vector<device_info> query_all_devices() noexcept;

    struct device_read_result
    {
        std::error_code    ec;
        device_input_state state;
    };
    [[nodiscard]] device_read_result read(const device_id) noexcept;

    // Provides a uniform control interface over physical input devices.
    class device_system
    {
    public:
        using listener_id = std::uint32_t;

        using device_change_callback = void (*)(const device_change_event&) noexcept;
        using device_state_callback  = void (*)(const device_input_state&) noexcept;


        struct configuration
        {
            std::chrono::milliseconds devices_poll_rate;
        };


        //explicit device_system();
        explicit device_system(const configuration&);
        ~device_system() noexcept;

        device_system(const device_system&) = delete;
        device_system& operator=(const device_system&) = delete;

        // Enable communications with a specific physical device.
        void enable(device_port) noexcept;

        // Disable communications with a specific physical device.
        void disable(device_port) noexcept;

        void attach_device_arrival_callback(listener_id, device_change_callback) noexcept;
        void detach_device_arrival_callback(listener_id) noexcept;

        void attach_device_removal_callback(listener_id, device_change_callback) noexcept;
        void detach_device_removal_callback(listener_id) noexcept;

        // Create a listener of device state notifications.
        void attach_state_callback(listener_id, device_id, device_state_callback) noexcept;

        // Destroy a listener of device state notifications.
        void detach_state_callback(listener_id, device_id) noexcept;

        void set(device_port, const device_output_state&) noexcept;

        // Run a single update cycle of all active devices.
        void update() noexcept;

#if !defined(DRAKO_API_RELEASE)
        // Prints list of registered listeners.
        void dbg_print_listeners() const;
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

        const configuration          _config;
        std::chrono::steady_clock    _clock;
        decltype(_clock)::time_point _last_poll_time; // time point of the last poll of connected devices

        struct _state_listeners_table_t
        {
            std::vector<listener_id>           ids;
            std::vector<device_state_callback> callbacks;
        } _state_event[XUSER_MAX_COUNT];

        struct _removal_listeners_table_t
        {
            std::vector<listener_id>            listeners;
            std::vector<device_change_callback> callbacks;
        } _dev_arrival;

        struct _arrival_listeners_table_t
        {
            std::vector<listener_id>            listeners;
            std::vector<device_change_callback> callbacks;
        } _dev_removal;
    };

} // namespace drako

#endif // !DRAKO_DEVICE_SYSTEM_HPP