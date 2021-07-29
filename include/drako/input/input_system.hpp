#pragma once
#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include "drako/core/typed_handle.hpp"
#include "drako/input/device_system.hpp"
#include "drako/input/device_types.hpp"
#include "drako/input/input_types.hpp"

#include <cassert>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace input
{
    struct Action
    {
        DRAKO_DEFINE_TYPED_ID(ID, std::uint32_t);
        using Callback = std::function<void()>;

        Action(const std::string_view name, Callback c, ID id) noexcept
            : name{ name }
            , reaction{ c }
            , instance{ id }
            , event{ std::hash<std::string_view>{}(name) } {}

        std::string name;
        Callback    reaction;
        const ID    instance;
        EventID     event;
    };


    /// @brief Runtime manager of the input system.
    class InputSystemRuntime
    {
    public:
        explicit InputSystemRuntime() = default;

        InputSystemRuntime(const InputSystemRuntime&) = delete;
        InputSystemRuntime& operator=(const InputSystemRuntime&) = delete;

        InputSystemRuntime(InputSystemRuntime&&) = delete;
        InputSystemRuntime& operator=(InputSystemRuntime&&) = delete;


        void create(const GamepadButtonBinding&);
        void create(const GamepadAxisBinding&);
        void create(const Action&);
        void create(const ActionMap&);

        void create_press_gesture(const BooleanControlID, const EventID);
        void create_press_gesture(const Gesture&);

        void create_release_gesture(const BooleanControlID, const EventID);
        void create_release_gesture(const Gesture&);

        void destroy(const Action::ID) noexcept;
        void destroy(const BindingID) noexcept;
        void destroy(const ActionMap&) noexcept;

        void enable(const Action::ID) noexcept;
        void enable(const ActionMap&) noexcept;

        void disable(const Action::ID) noexcept;
        void disable(const ActionMap&) noexcept;

        // Bind a named event with a physical key.
        //void bind(const std::string& interaction, const std::string& action) noexcept;

        // Bind a named event with a physical key.
        void bind(const std::string_view name, const VKey key) noexcept;

        void bind(const BindingID binding, const GamepadAxisID axis) noexcept;
        //void bind(const BindingID binding, const GamepadButtonID button) noexcept;

        void update() noexcept;

        // Run update cycle with incoming event.
        void update(const DeviceInputState&) noexcept;

        // Run update cycle with events generated from a device system.
        void update(const std::vector<DeviceInputState>&) noexcept;

#if !defined(DRAKO_API_RELEASE)
        void debug_print_actions() const noexcept;
        void debug_print_gamepad_bindings() const noexcept;
        void debug_print_on_press_table() const noexcept;
        void debug_print_on_release_table() const noexcept;
#endif

    private:
        DeviceInputState _last_state;

        std::vector<EventID>          _temp_event_buffer;
        std::vector<Action::Callback> _temp_invoke_buffer;

#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
        /*vvv Include bindings for the system main keyboard vvv*/

        struct _keyboard_keys_bindings_table
        {
            std::vector<BindingID>        binding; // unique id of the binding
            std::vector<KeyboardKeyID>    key;     // associated physical key on the keyboard
            std::vector<BooleanControlID> control; // associated virtual control
            std::vector<std::string>      name;    // debug-only friendly name
        } _keyboard_keys_bindings;
#endif

        struct _gamepad_button_bindings_table
        {
            std::vector<BindingID>        binding; // unique id of the binding instance
            std::vector<GamepadButtonID>  button;  // associated physical button on the gamepad
            std::vector<BooleanControlID> control; // associated virtual control
            std::vector<std::string>      name;    // debug-only friendly name
        } _gamepad_button_bindings;

        struct _gamepad_axes_bindings_table
        {
            std::vector<BindingID>     binding; // unique id of the binding instance
            std::vector<GamepadAxisID> axis;    // associated physical button on the gamepad
            std::vector<AxisControlID> control; // associated virtual control
            std::vector<std::string>   name;    // debug-only friendly name
        } _gamepad_axes_bindings;

        struct _on_press_table
        {
            // TODO: vvv these need to be threadsafe
            //std::vector<on_press>     pending_create;
            //std::vector<on_press::id> pending_destroy;
            //std::vector<on_press::id> pending_enable;
            //std::vector<on_press::id> pending_disable;
            // ^^^

            std::vector<EventID>          event;
            std::vector<BooleanControlID> control;
            std::vector<std::string>      name;
        } _on_press;

        struct _on_release_table
        {
            // TODO: vvv these need to be threadsafe
            //std::vector<on_release>     pending_create;
            //std::vector<on_release::id> pending_destroy;
            //std::vector<on_release::id> pending_enable;
            //std::vector<on_release::id> pending_disable;
            // ^^^

            std::vector<EventID>          event;
            std::vector<BooleanControlID> control;
            std::vector<std::string>      name;
        } _on_release;

        /*struct _on_hold_table
        {
            std::vector<on_hold::id> ids;
            std::vector<std::string> names;
            std::vector<action::id>  actions;
            std::vector<float>       duration;
            std::vector<float>       elapsed;
        } _on_hold;*/

        struct _actions_table
        {
            // TODO: thread safety
            std::vector<Action>     pending_create;
            std::vector<Action::ID> pending_destroy;
            std::vector<Action::ID> pending_enable;
            std::vector<Action::ID> pending_disable;
            // ^^^

            std::vector<Action::ID>       action;   // unique id of each action instance
            std::vector<EventID>          event;    // trigger event
            std::vector<Action::Callback> callback; // reaction to the trigger
            std::vector<std::string>      name;     // debug-only friendly name
        } _actions;
    };

} // namespace input

#endif // !INPUT_SYSTEM_HPP