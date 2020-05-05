#pragma once
#ifndef DRAKO_ENGINE_GESTURE_SYSTEM_HPP_
#define DRAKO_ENGINE_GESTURE_SYSTEM_HPP_

#include "drako/devel/assertion.hpp"
#include "drako/input/input_system_types.hpp"

#include <deque>
#include <string>
#include <string_view>
#include <vector>

namespace drako::engine::input
{
    using control_id      = std::uint16_t;
    using action_id       = std::uint16_t;
    using action_callback = void (*)(void);

    [[nodiscard]] inline constexpr bool is_valid(action_id id) noexcept
    {
        return id != 0;
    }


    enum class interaction
    {
        button_press,
        button_release,
        button_timed_hold
    };

    struct action_create_info
    {
        std::string     name;
        action_callback callback;
        action_id       id = new_action_id();
        interaction     user_interaction;

        action_create_info(const std::string_view name, action_callback cb, interaction i) noexcept
            : name(name), callback(cb), user_interaction(i)
        {
            DRAKO_ASSERT(!std::empty(name));
            DRAKO_ASSERT(cb != nullptr);
        }
    };

    struct binding
    {
        std::string control;
        control_id  id;

        binding(const std::string_view control, const std::string_view action) noexcept
            : control(control), id()
        {
            DRAKO_ASSERT(!std::empty(control));
            DRAKO_ASSERT(!std::empty(action));
        }
    };

    [[nodiscard]] inline constexpr control_id _map_button_to_control(size_t button) noexcept
    {
        return button;
    }

    using action_handle = std::uint16_t;

    [[nodiscard]] action_handle new_action_handle() noexcept
    {
        static action_handle _last = 0; // TODO: may need thread-safety in the future
        return ++_last;
    }


    class gesture_system
    {
    public:
        explicit gesture_system() noexcept = default;

        gesture_system(const gesture_system&) = delete;
        gesture_system& operator=(const gesture_system&) = delete;

        gesture_system(gesture_system&&) = delete;
        gesture_system& operator=(gesture_system&&) = delete;

        [[nodiscard]] action_id create(const action_create_info&) noexcept;

        void enable(const action_id) noexcept;
        void enable(const std::vector<action_id>&) noexcept;

        void disable(const action_id) noexcept;
        void disable(const std::vector<action_id>&) noexcept;

        void destroy(const action_id) noexcept;
        void destroy(const std::vector<action_id>&) noexcept;

        void bind(const binding&, const action_id) noexcept;
        //void unbind(const binding& b) noexcept;

        //void enable_action_group() noexcept;
        //void disable_action_group() noexcept;

        // Run update cycle with incoming event.
        void update(const input_device_state&) noexcept;

        // Run update cycle with events generated from an input system.
        void update(const std::vector<input_device_state>&) noexcept;

#if defined(DRAKO_DEBUG)
        void dbg_print_actions() const;

        void dbg_print_bindings() const;
#endif

    private:
        struct _action_create_cmd
        {
            action_create_info info;
            action_id          id;

            _action_create_cmd(const action_create_info& _info, action_id _id) noexcept
                : info(_info), id(_id)
            {
                DRAKO_ASSERT(is_valid(_id));
            }
        };


        input_device_state _last_state;

        /* queues for thread-safe concurrency */

        std::vector<_action_create_cmd> _pending_creates;
        std::vector<action_id>          _pending_destroys;
        std::vector<action_id>          _pending_enables;
        std::vector<action_id>          _pending_disables;


        std::vector<control_id>  _control_ids;
        std::vector<std::string> _control_names;

        std::vector<action_handle>   _action_handles;
        std::vector<std::string>     _action_names;
        std::vector<interaction>     _action_interactions;
        std::vector<action_callback> _action_callbacks;


        /* tables for currently enabled actions */

        std::vector<control_id>      _press_controls;
        std::vector<action_id>       _press_actions;
        std::vector<action_callback> _press_callbacks;

        std::vector<control_id>      _release_controls;
        std::vector<action_id>       _release_actions;
        std::vector<action_callback> _release_callbacks;


        /* tables for currently disabled actions */

        std::vector<control_id>      _disabled_press_controls;
        std::vector<action_id>       _disabled_press_actions;
        std::vector<action_callback> _disabled_press_callbacks;

        std::vector<control_id>      _disabled_release_controls;
        std::vector<action_id>       _disabled_release_actions;
        std::vector<action_callback> _disabled_release_callback;

        //std::vector<gesture_id>      _on_hold_id;
        //std::vector<control_id>      _on_hold_control;
        //std::vector<action_callback> _on_hold_action;
        //std::vector<float>           _on_hold_duration;
        //std::vector<float>           _on_hold_elapsed;

        void _process_pending_creates() noexcept;
        void _process_pending_destroys() noexcept;
        void _process_pending_state_changes() noexcept;
    };


    // RAII wrapper for action instances.
    class action
    {
    public:
        action(gesture_system& s, const action_create_info& info) noexcept
            : _info(info), _system(s), _handle(s.create(info))
        {
        }

        action(const action&) = delete;
        action& operator=(const action&) = delete;

        action(action&&) = delete;
        action& operator=(action&&) = delete;

        ~action() noexcept
        {
            _system.destroy(_handle);
        }

    private:
        action_create_info _info;
        gesture_system&    _system;
        action_id          _handle;
    };

} // namespace drako::engine::input

#endif // !DRAKO_ENGINE_GESTURE_SYSTEM_HPP_