#pragma once
#ifndef DRAKO_INPUT_SYSTEM_HPP
#define DRAKO_INPUT_SYSTEM_HPP

#include <cstdint>
#include <vector>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/system/native_mouse_device.hpp"
#include "drako/devel/logging.hpp"
#include "drako/input/input_device.hpp"

namespace drako
{
    // TYPEDEF
    // Uniquely identifies an input device.
    using device_id = std::uint32_t;


    struct virtual_input_event
    {
    };

    class virtual_input_device
    {
    public:
        virtual std::vector<virtual_input_event> poll_events() noexcept;
    };


    struct virtual_binding_set
    {
        using physical_key_id = std::uint16_t;
        using virtual_key_id  = std::uint16_t;

        std::vector<std::tuple<physical_key_id, virtual_key_id>> bindings;
    };


    // Provides a uniform control interface over physical input devices.
    class virtual_input_system
    {
    public:
        static void destroy_device(device_id device) noexcept;

        // Enables the selected device.
        // Enabled devices produces events and responds to commands.
        static void enable_device(device_id device) noexcept;

        // Disables the selected device.
        // Disabled devices don't produce events and don't responds to commands.
        static void disable_device(device_id device) noexcept;

        void update() noexcept;

        void update(device_id id) noexcept;

    private:
        std::vector<device_id>            _device_ids;
        std::vector<virtual_input_device> _device_handles;
        std::vector<virtual_binding_set>  _device_mapping_tables;
    };

    void virtual_input_system::update() noexcept
    {
        // foreach device process event queue
        for (const auto& device : _device_handles)
        {
        }
    }

    void virtual_input_system::update(device_id id) noexcept
    {
        const auto it = std::find(std::cbegin(_device_ids), std::cend(_device_ids), id);
        if (it == std::cend(_device_ids))
        {
            DRAKO_LOG_ERROR("[Drako] Can't locate device with specified id");
            return;
        }

        const auto index  = std::distance(std::cbegin(_device_ids), it);
        auto       device = _device_handles[index];
        const auto events = device.poll_events();

        if (std::empty(events))
            return;

        const auto translation_table = _device_mapping_tables[index];
        for (const auto& event : events)
        {
        }
    }

} // namespace drako

#endif // !DRAKO_INPUT_SYSTEM_HPP