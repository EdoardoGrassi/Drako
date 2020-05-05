#pragma once
#include "drako/input/gesture_system.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

namespace drako::engine::input
{
    void gesture_system::_process_pending_creates() noexcept
    {
        for (const auto& create : _pending_creates)
        {
            switch (create.info.user_interaction)
            {
                case interaction::button_press:
                {
                    _press_actions.push_back(create.id);
                    _press_controls.push_back(create.)
                }

                case interaction::button_release:
                {
                }

                default:
                    DRAKO_LOG_ERROR("Unknown interaction category");
            }
        }
    }

    void gesture_system::_process_pending_destroys() noexcept
    {
    }

    void gesture_system::_process_pending_state_changes() noexcept
    {
    }

    [[nodiscard]] action_id gesture_system::create(const action_create_info& info) noexcept
    {
        const auto               id = new_action_handle();
        const _action_create_cmd cmd{ info, id };
        _pending_creates.push_back(cmd);
        return id;
    }

    void gesture_system::destroy(const action_handle a) noexcept
    {
        DRAKO_ASSERT(is_valid(a), "Invalid handle");
        _pending_destroys.push_back(a);
    }

    void gesture_system::enable(const action_handle a) noexcept
    {
        DRAKO_ASSERT(is_valid(a), "Invalid handle");
        _pending_enables.push_back(a);
    }

    void gesture_system::disable(const action_handle a) noexcept
    {
        DRAKO_ASSERT(is_valid(a), "Invalid handle");
        _pending_disables.push_back(a);
    }

    void gesture_system::update(const input_device_state& state) noexcept
    {
        _process_pending_creates();
        _process_pending_destroys();
        _process_pending_state_changes();

        std::vector<control_id> pressed_controls;
        std::vector<control_id> released_controls;

        const auto changed_from_last_update = _last_state.buttons ^ state.buttons;

        const auto pressed = changed_from_last_update & state.buttons;
        for (decltype(std::size(pressed)) i = 0; i < std::size(pressed); ++i)
        {
            if (pressed.test(i))
                pressed_controls.push_back(_map_button_to_control(i));
        }

        const auto released = changed_from_last_update & (~state.buttons);
        for (decltype(std::size(released)) i = 0; i < std::size(released); ++i)
        {
            if (released.test(i))
                released_controls.push_back(_map_button_to_control(i));
        }
    }

} // namespace drako::engine::input