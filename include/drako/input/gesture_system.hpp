#pragma once
#ifndef DRAKO_GESTURE_SYSTEM_HPP_
#define DRAKO_GESTURE_SYSTEM_HPP_

#include "drako/input/input_system.hpp"

#include <string_view>
#include <vector>

namespace drako::engine
{
    // Runtime Identifier object
    template <typename T>
    struct rid;

    using rid_control     = const std::uint16_t;
    using rid_action      = const std::uint16_t;
    using rid_gesture     = const std::uint16_t;
    using action_callback = void (*)(void);

    enum class gesture_type
    {
        button_press,
        button_release,
        button_timed_hold
    };

    struct gesture_instance
    {
        rid_gesture     id;
        std::string     format;
        gesture_type    type;
        action_callback action;
    };


    class gesture_system
    {
    public:
        explicit gesture_system() noexcept = default;

        gesture_system(const gesture_system&) = delete;
        gesture_system& operator=(const gesture_system&) = delete;

        gesture_system(gesture_system&&) = delete;
        gesture_system& operator=(gesture_system&&) = delete;


        void create_gesture(const rid_gesture id, const gesture_instance& gesture) noexcept;
        void create_gesture(const std::string_view name, const gesture_instance& gesture) noexcept;

        void destroy_gesture(const rid_gesture id) noexcept;
        void destroy_gesture(const std::string_view name) noexcept;

        void enable_action_group() noexcept;

        void disable_action_group() noexcept;

        // run update cycle with events generated from an input system
        void update(const std::vector<input_event>& events) noexcept;


    private:
        std::vector<rid_gesture>     _on_press_id;
        std::vector<rid_control>     _on_press_control;
        std::vector<action_callback> _on_press_action;

        std::vector<rid_gesture>     _on_release_id;
        std::vector<rid_control>     _on_release_control;
        std::vector<action_callback> _on_release_action;

        std::vector<rid_control>     _on_hold_id;
        std::vector<rid_control>     _on_hold_control;
        std::vector<action_callback> _on_hold_action;
        std::vector<float>           _on_hold_timeout;
    };

    void gesture_system::create_gesture(const rid_gesture id, const gesture_instance& gesture) noexcept
    {
    }

    void gesture_system::create_gesture(const std::string_view name, const gesture_instance& gesture) noexcept
    {
    }

    void gesture_system::destroy_gesture(const rid_gesture id)
    {
        const auto it = std::find(std::cbegin(_on_press_id), std::cend(_on_press_id), id);
        if (it != std::cend(_on_press_id))
        {
            const auto offset = std::distance(std::cbegin(_on_press_id), it);
            _on_press_id.erase(std::begin(_on_press_id) + offset);
            _on_press_control.erase(std::begin(_on_press_control) + offset);
            _on_press_action.erase(std::begin(_on_press_action) + offset);
        }
    }

    void gesture_system::update(const std::vector<input_event>& events) noexcept
    {
        std::vector<rid_control> press_events;
        std::sort(std::begin(press_events), std::end(press_events));

        for (auto i = 0, j = 0; i < std::size(press_events); ++i)
        {
            while (press_events[i] > _on_press_control[j])
                ++j;
            if (press_events[i] == _on_press_control[j])
                _on_press_action[j]();
        }

        std::vector<rid_control> release_events;
        std::sort(std::begin(release_events), std::end(release_events));

        for (auto i = 0, j = 0; i < std::size(release_events); ++i)
        {
            while (release_events[i] > _on_release_control[j])
                ++j;
            if (release_events[i] == _on_release_control[j])
                _on_release_action[j];
        }
    }

} // namespace drako::engine

#endif // !DRAKO_GESTURE_SYSTEM_HPP_