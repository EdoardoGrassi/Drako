#pragma once
#ifndef DRAKO_INPUT_SYSTEM_HPP_
#define DRAKO_INPUT_SYSTEM_HPP_

#include <cstdint>
#include <vector>

#include "drako/devel/logging.hpp"
#include "drako/input/input_device.hpp"
#include "drako/system/native_mouse_device.hpp"

namespace drako::engine
{
    // TYPEDEF
    // Uniquely identifies an input device.
    using device_id = const std::uint32_t;


    struct _native_input_event;

    struct input_event
    {
    };


    class input_device_interface
    {
    public:
        virtual std::vector<float> axis_states() noexcept;

        virtual std::vector<bool> button_states() noexcept;

        virtual std::vector<_native_input_event> poll_events() noexcept;
    };


    struct binding_set
    {
        using physical_key_id = std::uint16_t;
        using virtual_key_id  = std::uint16_t;

        //std::vector<std::tuple<physical_key_id, virtual_key_id>> bindings;

        std::vector<std::uint16_t> axis_control_id;
        std::vector<std::uint16_t> axis_action_id;
        std::vector<std::uint16_t> axis_action_group_id;

        std::vector<std::uint16_t> btn_control_id;
        std::vector<std::uint16_t> btn_action_id;
        std::vector<std::uint16_t> btn_action_group_id;
    };



    struct input_device_state
    {
        float         axis[10];
        std::uint64_t btns;
    };


    // Provides a uniform control interface over physical input devices.
    class input_system
    {
    public:
        explicit input_system() = default;

        input_system(const input_system&) = delete;
        input_system& operator=(const input_system&) = delete;

        input_system(input_system&&) = delete;
        input_system& operator=(input_system&&) = delete;


        device_id create_device(const binding_set& bindings) noexcept;

        void create_device_layout(const input_device_layout& layout) noexcept;

        void destroy_device(device_id device) noexcept;

        // Enables the selected device.
        // Enabled devices produces events and responds to commands.
        void enable_device(device_id device) noexcept;

        // Disables the selected device.
        // Disabled devices don't produce events and don't responds to commands.
        void disable_device(device_id device) noexcept;

        void enable_bindings_group() noexcept;

        void disable_bindings_group() noexcept;

        void update() noexcept;

        void update(device_id id) noexcept;

    private:
        struct _input_device_context
        {
            std::vector<action_callback> actions;
        };

        static std::remove_const_t<device_id> _last_device;

        std::vector<device_id>              _device_ids;
        std::vector<input_device_interface> _device_handles;
        std::vector<binding_set>            _device_mapping_tables;

        std::vector<_input_device_context> _devices;

        //std::vector<action_id>       _actions_id;
        //std::vector<action_callback> _actions_callbacks;


        [[nodiscard]] device_id _new_device_id() { return ++_last_device; }
    };

    device_id input_system::create_device(const binding_set& bindings) noexcept
    {
        const auto id = _new_device_id();
        _device_ids.emplace_back(id);
    }

    void input_system::create_device_layout(const input_device_layout& layout) noexcept
    {
    }

    void input_system::update() noexcept
    {
        // foreach device process event queue
        for (auto& device : _device_handles)
        {
            const auto events = device.poll_events();
            for (const auto& e : events)
            {
            }
        }
    }

    void input_system::update(device_id id) noexcept
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

} // namespace drako::engine

#endif // !DRAKO_INPUT_SYSTEM_HPP_